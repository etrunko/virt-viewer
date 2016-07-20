/*
 * Virt Viewer: A virtual machine console viewer
 *
 * Copyright (C) 2016 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <config.h>

#include <stdlib.h>
#include <glib/gi18n.h>

#include "remote-viewer-iso-list-dialog.h"
#include "virt-viewer-util.h"
#include "ovirt-foreign-menu.h"

G_DEFINE_TYPE(RemoteViewerISOListDialog, remote_viewer_iso_list_dialog, GTK_TYPE_DIALOG)

#define DIALOG_PRIVATE(o) \
        (G_TYPE_INSTANCE_GET_PRIVATE((o), REMOTE_VIEWER_TYPE_ISO_LIST_DIALOG, RemoteViewerISOListDialogPrivate))

struct _RemoteViewerISOListDialogPrivate
{
    GtkHeaderBar *header_bar;
    GtkListStore *list_store;
    GtkWidget *stack;
    GtkWidget *tree_view;
    OvirtForeignMenu *foreign_menu;
};

enum RemoteViewerISOListDialogModel
{
    ISO_IS_ACTIVE = 0,
    ISO_NAME,
    FONT_WEIGHT,
};

void remote_viewer_iso_list_dialog_toggled(GtkCellRendererToggle *cell_renderer, gchar *path, gpointer user_data);
void remote_viewer_iso_list_dialog_row_activated(GtkTreeView *view, GtkTreePath *path, GtkTreeViewColumn *col, gpointer user_data);

static void
remote_viewer_iso_list_dialog_dispose(GObject *object)
{
    RemoteViewerISOListDialog *self = REMOTE_VIEWER_ISO_LIST_DIALOG(object);
    RemoteViewerISOListDialogPrivate *priv = self->priv;

    if (priv->foreign_menu) {
        g_signal_handlers_disconnect_by_data(priv->foreign_menu, object);
        g_clear_object(&priv->foreign_menu);
    }
    G_OBJECT_CLASS(remote_viewer_iso_list_dialog_parent_class)->dispose(object);
}

static void
remote_viewer_iso_list_dialog_class_init(RemoteViewerISOListDialogClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    g_type_class_add_private(klass, sizeof(RemoteViewerISOListDialogPrivate));

    object_class->dispose = remote_viewer_iso_list_dialog_dispose;
}

static void
remote_viewer_iso_list_dialog_show_files(RemoteViewerISOListDialog *self)
{
    RemoteViewerISOListDialogPrivate *priv = self->priv = DIALOG_PRIVATE(self);
    gtk_stack_set_visible_child_full(GTK_STACK(priv->stack), "iso-list",
                                     GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT);
    gtk_dialog_set_response_sensitive(GTK_DIALOG(self), GTK_RESPONSE_NONE, TRUE);
}

static void
remote_viewer_iso_list_dialog_set_subtitle(RemoteViewerISOListDialog *self, const char *iso_name)
{
    RemoteViewerISOListDialogPrivate *priv = self->priv;
    gchar *subtitle = NULL;

    if (iso_name && strlen(iso_name) != 0)
        subtitle = g_strdup_printf(_("Current: %s"), iso_name);

    gtk_header_bar_set_subtitle(priv->header_bar, subtitle);
    g_free(subtitle);
}

static void
remote_viewer_iso_list_dialog_foreach(char *name, RemoteViewerISOListDialog *self)
{
    RemoteViewerISOListDialogPrivate *priv = self->priv;
    char * current_iso = ovirt_foreign_menu_get_current_iso_name(self->priv->foreign_menu);
    gboolean active = g_strcmp0(current_iso, name) == 0;
    gint weight = active ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL;
    GtkTreeIter iter;

    gtk_list_store_append(priv->list_store, &iter);
    gtk_list_store_set(priv->list_store, &iter,
                       ISO_IS_ACTIVE, active,
                       ISO_NAME, name,
                       FONT_WEIGHT, weight, -1);

    if (active) {
        GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(priv->list_store), &iter);
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), path, NULL, FALSE);
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(priv->tree_view), path, NULL, TRUE, 0.5, 0.5);
        gtk_tree_path_free(path);
        remote_viewer_iso_list_dialog_set_subtitle(self, current_iso);
    }

    free(current_iso);
}

static void
remote_viewer_iso_list_dialog_refresh_iso_list(RemoteViewerISOListDialog *self,
                                               gboolean refreshing)
{
    RemoteViewerISOListDialogPrivate *priv = self->priv;
    GList *iso_list;

    if (refreshing) {
        gtk_list_store_clear(priv->list_store);
        ovirt_foreign_menu_start(priv->foreign_menu);
        return;
    }

    iso_list = ovirt_foreign_menu_get_iso_names(priv->foreign_menu);
    g_list_foreach(iso_list, (GFunc) remote_viewer_iso_list_dialog_foreach, self);
    remote_viewer_iso_list_dialog_show_files(self);
}

static void
remote_viewer_iso_list_dialog_response(GtkDialog *dialog,
                                       gint response_id,
                                       gpointer user_data G_GNUC_UNUSED)
{
    RemoteViewerISOListDialog *self = REMOTE_VIEWER_ISO_LIST_DIALOG(dialog);
    RemoteViewerISOListDialogPrivate *priv = self->priv;

    if (response_id != GTK_RESPONSE_NONE)
        return;

    remote_viewer_iso_list_dialog_set_subtitle(self, NULL);
    gtk_stack_set_visible_child_full(GTK_STACK(priv->stack), "status",
                                     GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT);
    gtk_dialog_set_response_sensitive(GTK_DIALOG(self), GTK_RESPONSE_NONE, FALSE);
    remote_viewer_iso_list_dialog_refresh_iso_list(self, TRUE);
}

void
remote_viewer_iso_list_dialog_toggled(GtkCellRendererToggle *cell_renderer G_GNUC_UNUSED,
                                      gchar *path,
                                      gpointer user_data)
{
    RemoteViewerISOListDialog *self = REMOTE_VIEWER_ISO_LIST_DIALOG(user_data);
    RemoteViewerISOListDialogPrivate *priv = self->priv;
    GtkTreeModel *model = GTK_TREE_MODEL(priv->list_store);
    GtkTreePath *tree_path = gtk_tree_path_new_from_string(path);
    GtkTreeIter iter;
    gboolean active;
    gchar *name;

    gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), tree_path, NULL, FALSE);
    gtk_tree_model_get_iter(model, &iter, tree_path);
    gtk_tree_model_get(model, &iter,
                       ISO_IS_ACTIVE, &active,
                       ISO_NAME, &name, -1);

    gtk_dialog_set_response_sensitive(GTK_DIALOG(self), GTK_RESPONSE_NONE, FALSE);
    gtk_widget_set_sensitive(priv->tree_view, FALSE);

    ovirt_foreign_menu_set_current_iso_name(priv->foreign_menu, active ? NULL : name);
    gtk_tree_path_free(tree_path);
    g_free(name);
}

void
remote_viewer_iso_list_dialog_row_activated(GtkTreeView *view G_GNUC_UNUSED,
                                            GtkTreePath *path,
                                            GtkTreeViewColumn *col G_GNUC_UNUSED,
                                            gpointer user_data)
{
    gchar *path_str = gtk_tree_path_to_string(path);
    remote_viewer_iso_list_dialog_toggled(NULL, path_str, user_data);
    g_free(path_str);
}

static void
remote_viewer_iso_list_dialog_init(RemoteViewerISOListDialog *self)
{
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(self));
    RemoteViewerISOListDialogPrivate *priv = self->priv = DIALOG_PRIVATE(self);
    GtkBuilder *builder = virt_viewer_util_load_ui("remote-viewer-iso-list.ui");
    GtkCellRendererToggle *cell_renderer;
    GtkWidget *button, *image;

    gtk_builder_connect_signals(builder, self);

    priv->header_bar = GTK_HEADER_BAR(gtk_dialog_get_header_bar(GTK_DIALOG(self)));
    gtk_header_bar_set_has_subtitle(priv->header_bar, TRUE);

    priv->stack = GTK_WIDGET(gtk_builder_get_object(builder, "stack"));
    gtk_box_pack_start(GTK_BOX(content), priv->stack, TRUE, TRUE, 0);

    priv->list_store = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore"));
    priv->tree_view = GTK_WIDGET(gtk_builder_get_object(builder, "view"));
    cell_renderer = GTK_CELL_RENDERER_TOGGLE(gtk_builder_get_object(builder, "cellrenderertoggle"));
    gtk_cell_renderer_toggle_set_radio(cell_renderer, TRUE);
    gtk_cell_renderer_set_padding(GTK_CELL_RENDERER(cell_renderer), 6, 6);

    g_object_unref(builder);

    button = gtk_dialog_add_button(GTK_DIALOG(self), "", GTK_RESPONSE_NONE);
    image = gtk_image_new_from_icon_name("view-refresh-symbolic", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(button), image);
    gtk_button_set_always_show_image(GTK_BUTTON(button), TRUE);

    gtk_dialog_set_response_sensitive(GTK_DIALOG(self), GTK_RESPONSE_NONE, FALSE);
    g_signal_connect(self, "response", G_CALLBACK(remote_viewer_iso_list_dialog_response), NULL);
}

static void
ovirt_foreign_menu_notify_file(OvirtForeignMenu *foreign_menu,
                               GParamSpec *pspec G_GNUC_UNUSED,
                               RemoteViewerISOListDialog *self)
{
    RemoteViewerISOListDialogPrivate *priv = self->priv;
    GtkTreeModel *model = GTK_TREE_MODEL(priv->list_store);
    char *current_iso = ovirt_foreign_menu_get_current_iso_name(foreign_menu);
    GtkTreeIter iter;
    gchar *name;
    gboolean active, match = FALSE;

    if (!gtk_tree_model_get_iter_first(model, &iter))
        goto end;

    do {
        gtk_tree_model_get(model, &iter,
                           ISO_IS_ACTIVE, &active,
                           ISO_NAME, &name, -1);
        match = g_strcmp0(current_iso, name) == 0;

        /* iso is not active anymore */
        if (active && !match) {
            gtk_list_store_set(priv->list_store, &iter,
                               ISO_IS_ACTIVE, FALSE,
                               FONT_WEIGHT, PANGO_WEIGHT_NORMAL, -1);
        } else if (match) {
            gtk_list_store_set(priv->list_store, &iter,
                               ISO_IS_ACTIVE, TRUE,
                               FONT_WEIGHT, PANGO_WEIGHT_BOLD, -1);
        }

        g_free(name);
    } while (gtk_tree_model_iter_next(model, &iter));

    remote_viewer_iso_list_dialog_set_subtitle(self, current_iso);
    gtk_dialog_set_response_sensitive(GTK_DIALOG(self), GTK_RESPONSE_NONE, TRUE);
    gtk_widget_set_sensitive(priv->tree_view, TRUE);

end:
    free(current_iso);
}

static void
ovirt_foreign_menu_notify_files(OvirtForeignMenu *foreign_menu G_GNUC_UNUSED,
                                GParamSpec *pspec G_GNUC_UNUSED,
                                RemoteViewerISOListDialog *self)
{
    remote_viewer_iso_list_dialog_refresh_iso_list(self, FALSE);
}


GtkWidget *
remote_viewer_iso_list_dialog_new(GtkWindow *parent, GObject *foreign_menu)
{
    GtkWidget *dialog;
    RemoteViewerISOListDialog *self;

    g_return_val_if_fail(foreign_menu != NULL, NULL);

    dialog = g_object_new(REMOTE_VIEWER_TYPE_ISO_LIST_DIALOG,
                          "title", _("Change CD"),
                          "transient-for", parent,
                          "border-width", 18,
                          "default-width", 400,
                          "default-height", 300,
                          "use-header-bar", TRUE,
                          NULL);

    self = REMOTE_VIEWER_ISO_LIST_DIALOG(dialog);
    self->priv->foreign_menu = OVIRT_FOREIGN_MENU(g_object_ref(foreign_menu));

    g_signal_connect(foreign_menu,
                     "notify::file",
                     (GCallback)ovirt_foreign_menu_notify_file,
                     dialog);
    g_signal_connect(foreign_menu,
                     "notify::files",
                     (GCallback)ovirt_foreign_menu_notify_files,
                     dialog);

    remote_viewer_iso_list_dialog_refresh_iso_list(self, TRUE);
    return dialog;
}
