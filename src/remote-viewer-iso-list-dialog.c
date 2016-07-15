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

#include <glib/gi18n.h>

#include "remote-viewer-iso-list-dialog.h"
#include "virt-viewer-util.h"
#include "ovirt-foreign-menu.h"

G_DEFINE_TYPE(RemoteViewerISOListDialog, remote_viewer_iso_list_dialog, GTK_TYPE_DIALOG)

#define DIALOG_PRIVATE(o) \
        (G_TYPE_INSTANCE_GET_PRIVATE((o), REMOTE_VIEWER_TYPE_ISO_LIST_DIALOG, RemoteViewerISOListDialogPrivate))

struct _RemoteViewerISOListDialogPrivate
{
    GtkWidget *stack;
    OvirtForeignMenu *foreign_menu;
};

static void
remote_viewer_iso_list_dialog_dispose(GObject *object)
{
    RemoteViewerISOListDialog *self = REMOTE_VIEWER_ISO_LIST_DIALOG(object);
    RemoteViewerISOListDialogPrivate *priv = self->priv;

    g_clear_object(&priv->foreign_menu);
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
remote_viewer_iso_list_dialog_response(GtkDialog *dialog,
                                       gint response_id,
                                       gpointer user_data G_GNUC_UNUSED)
{
    RemoteViewerISOListDialog *self = REMOTE_VIEWER_ISO_LIST_DIALOG(dialog);
    RemoteViewerISOListDialogPrivate *priv = self->priv;

    if (response_id != GTK_RESPONSE_NONE)
        return;

    gtk_stack_set_visible_child_full(GTK_STACK(priv->stack), "status",
                                     GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT);
    gtk_dialog_set_response_sensitive(GTK_DIALOG(self), GTK_RESPONSE_NONE, FALSE);
    gtk_dialog_set_response_sensitive(GTK_DIALOG(self), GTK_RESPONSE_CLOSE, FALSE);
}

static void
remote_viewer_iso_list_dialog_init(RemoteViewerISOListDialog *self)
{
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(self));
    RemoteViewerISOListDialogPrivate *priv = self->priv = DIALOG_PRIVATE(self);
    GtkBuilder *builder = virt_viewer_util_load_ui("remote-viewer-iso-list.ui");

    gtk_builder_connect_signals(builder, self);

    priv->stack = GTK_WIDGET(gtk_builder_get_object(builder, "stack"));
    gtk_box_pack_start(GTK_BOX(content), priv->stack, TRUE, TRUE, 0);

    g_object_unref(builder);

    gtk_dialog_add_buttons(GTK_DIALOG(self),
                           _("Refresh"), GTK_RESPONSE_NONE,
                           _("Close"), GTK_RESPONSE_CLOSE,
                           NULL);

    gtk_dialog_set_default_response(GTK_DIALOG(self), GTK_RESPONSE_CLOSE);
    gtk_dialog_set_response_sensitive(GTK_DIALOG(self), GTK_RESPONSE_NONE, FALSE);
    gtk_dialog_set_response_sensitive(GTK_DIALOG(self), GTK_RESPONSE_CLOSE, FALSE);
    g_signal_connect(self, "response", G_CALLBACK(remote_viewer_iso_list_dialog_response), NULL);
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
                          NULL);

    self = REMOTE_VIEWER_ISO_LIST_DIALOG(dialog);
    self->priv->foreign_menu = OVIRT_FOREIGN_MENU(g_object_ref(foreign_menu));
    return dialog;
}
