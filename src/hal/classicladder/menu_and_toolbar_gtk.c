/* Classic Ladder Project */
/* Copyright (C) 2001-2010 Marc Le Douarain */
/* http://membres.lycos.fr/mavati/classicladder/ */
/* http://www.sourceforge.net/projects/classicladder */
/* December 2010 */
/* ------------------------------------------------------ */
/* Menus & Toolbar - GTK window                           */
/* ------------------------------------------------------ */
/* Many thanks to Heli Tejedor for his modified version   */
/* with idea of possible menus and toolbar feature        */
/* ------------------------------------------------------ */
/* This library is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU Lesser General Public */
/* License as published by the Free Software Foundation; either */
/* version 2.1 of the License, or (at your option) any later version. */

/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU */
/* Lesser General Public License for more details. */

/* You should have received a copy of the GNU Lesser General Public */
/* License along with this library; if not, write to the Free Software */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include <gtk/gtk.h>

#include "classicladder.h"
#include "classicladder_gtk.h"
#include "print_gtk.h"
#include "edit_gtk.h"
#include "symbols_gtk.h"
#include "manager_gtk.h"
#include "config_gtk.h"
#include "spy_vars_gtk.h"
//#include "log_gtk.h"

GtkUIManager * uiManager;

static GtkActionEntry ActionEntriesArray[ ] =
{	{ "FileMenuAction", NULL, "File" },
	{ "NewAction", GTK_STOCK_NEW, "New", "<Control>N", "Create a new project", G_CALLBACK( DoActionConfirmNewProject ) },
	{ "LoadAction", GTK_STOCK_OPEN, "Load", "<Control>L", "Load an existing project", G_CALLBACK( DoActionLoadProject ) },
	{ "SaveAction", GTK_STOCK_SAVE, "Save", "<Control>S", "Save current project", G_CALLBACK( DoActionSave ) },
	{ "SaveAsAction", NULL, "Save As...", "<Control>A", "Save project to another file", G_CALLBACK( DoActionSaveAs ) },
	{ "ExportMenuAction", NULL, "Export to", NULL, NULL, NULL },
	{ "ExportSvgAction", NULL, "Svg", NULL, NULL, G_CALLBACK( DoActionExportSvg ) },
	{ "ExportPngAction", NULL, "Png", NULL, NULL, G_CALLBACK( DoActionExportPng ) },
	{ "CopyToClipboardAction", NULL, "Clipboard", "<Control>C", NULL, G_CALLBACK( DoActionCopyToClipboard ) },
	{ "PreviewAction", GTK_STOCK_PRINT_PREVIEW, "Preview", NULL, NULL, G_CALLBACK( PrintPreviewGtk ) },
	{ "PrintAction", GTK_STOCK_PRINT, "Print", "<Control>P", "Print current section", G_CALLBACK( PrintGtk ) },
	{ "QuitAction", GTK_STOCK_QUIT, "Quit", "<Control>Q", NULL, G_CALLBACK( ConfirmQuit ) },

	{ "ViewMenuAction", NULL, "View" },

	{ "PLCAction", NULL, "PLC" },
	{ "RunStopAction", GTK_STOCK_EXECUTE, "RunStop", NULL, "Start/stop logic", G_CALLBACK( DoFlipFlopRunStop ) },
	{ "ResetAction", GTK_STOCK_REFRESH, "Reset", NULL, "Reset logic", G_CALLBACK( DoActionResetAndConfirmIfRunning ) },
	{ "ConfigurationAction", GTK_STOCK_PREFERENCES, "Configuration", NULL, "Configuration (sizes, i/o, ...)", G_CALLBACK( OpenConfigWindowGtk ) },

	{ "HelpMenuAction", NULL, "Help" },
	{ "AboutAction", NULL, "About", "F1", NULL, G_CALLBACK( DoActionAboutClassicLadder ) },	
};
static GtkToggleActionEntry ToggleActionEntriesArray[ ] =
{	{ "ViewSectionsAction", GTK_STOCK_DND_MULTIPLE, "Sections window", "F2", "View sections manager window", G_CALLBACK( OpenManagerWindow ), TRUE },
	{ "ViewEditorAction", GTK_STOCK_EDIT, "Editor window", "F3", "View editor window", G_CALLBACK( OpenEditWindow ), FALSE },
	{ "ViewSymbolsAction", GTK_STOCK_INDEX, "Symbols window", "F4", "View symbols window", G_CALLBACK( OpenSymbolsWindow ), FALSE },
	{ "ViewBoolVarsAction", NULL, "Bit Status Window", "F5", NULL, G_CALLBACK( OpenSpyBoolVarsWindow ), FALSE },
	{ "ViewFreeVarsAction", NULL, "Watch Window", "F6", NULL, G_CALLBACK( OpenSpyFreeVarsWindow ), FALSE },
	//{ "ViewLogAction", NULL, "Log window", "F7", NULL, G_CALLBACK( OpenLogBookWindow ), FALSE },
};

static const gchar *ClassicLadder_ui_strings = 
"<ui>"
"	<menubar name='MenuBar'>"
"		<menu action='FileMenuAction'>"
"			<menuitem action='NewAction' />"
"			<menuitem action='LoadAction' />"
"			<menuitem action='SaveAction' />"
"			<menuitem action='SaveAsAction' />"
"			<separator />"
	"		<menu action='ExportMenuAction'>"
"				<menuitem action='ExportSvgAction' />"
"				<menuitem action='ExportPngAction' />"
"				<menuitem action='CopyToClipboardAction' />"
	"		</menu>"
"			<separator />"
"			<menuitem action='PreviewAction' />"
"			<menuitem action='PrintAction' />"
"			<separator />"
"			<menuitem action='QuitAction' />"
"		</menu>"
"		<menu action='ViewMenuAction'>"
"			<menuitem action='ViewSectionsAction' />"
"			<menuitem action='ViewEditorAction' />"
"			<menuitem action='ViewSymbolsAction' />"
"			<menuitem action='ViewBoolVarsAction' />"
"			<menuitem action='ViewFreeVarsAction' />"
//XXX Log functionality, not implemented.
//"			<menuitem action='ViewLogAction' />"
"		</menu>"
"		<menu action='PLCAction'>"
"			<menuitem action='RunStopAction' />"
"			<menuitem action='ResetAction' />"
"			<menuitem action='ConfigurationAction' />"
"		</menu>"
"		<menu action='HelpMenuAction'>"
"			<menuitem action='AboutAction' />"
"		</menu>"
"	</menubar>"
"	<toolbar name='ToolBar'>"
"		<toolitem action='NewAction' />"
"		<toolitem action='LoadAction' />"
"		<toolitem action='SaveAction' />"
"		<separator />"
"		<toolitem action='PrintAction' />"
"		<separator />"
"		<toolitem action='RunStopAction' />"
"		<toolitem action='ResetAction' />"
"		<toolitem action='ConfigurationAction' />"
"		<separator />"
"		<toolitem action='ViewSectionsAction' />"
"		<toolitem action='ViewEditorAction' />"
"		<toolitem action='ViewSymbolsAction' />"
"		<separator />"
"	</toolbar>"
"</ui>";

GtkUIManager * InitMenusAndToolBar( GtkWidget *vbox )
{
	GtkActionGroup * ActionGroup;
	GError *error = NULL;
	uiManager = gtk_ui_manager_new( );
	
	ActionGroup = gtk_action_group_new( "ClassicLadderActionGroup" );
	gtk_action_group_add_actions( ActionGroup, ActionEntriesArray, G_N_ELEMENTS( ActionEntriesArray ), NULL );
	gtk_action_group_add_toggle_actions( ActionGroup, ToggleActionEntriesArray, G_N_ELEMENTS( ToggleActionEntriesArray ), NULL/*user_data*/ );
	
	gtk_ui_manager_insert_action_group( uiManager, ActionGroup, 0 );
	if (!gtk_ui_manager_add_ui_from_string( uiManager, ClassicLadder_ui_strings, -1/*length*/, &error ))
	{
		g_message ("Failed to build gtk menus: %s", error->message);
		g_error_free (error);
	}
	else
	{
		GtkWidget * ToolBarWidget = gtk_ui_manager_get_widget( uiManager, "/ToolBar" );
		gtk_box_pack_start( GTK_BOX(vbox), gtk_ui_manager_get_widget( uiManager, "/MenuBar" ), FALSE, FALSE, 0 );
		//do not display text under icons (seems to be the case per default under Windows...)
		gtk_toolbar_set_style( GTK_TOOLBAR(ToolBarWidget), GTK_TOOLBAR_ICONS );
//		gtk_toolbar_set_style( GTK_TOOLBAR(ToolBarWidget), GTK_TOOLBAR_BOTH );
		gtk_box_pack_start( GTK_BOX(vbox), ToolBarWidget, FALSE, FALSE, 0 );
	}
	return uiManager;
}

// called at startup (if window saved open or not), and when window closed...
void SetToogleMenuForSectionsManagerWindow( gboolean OpenedWin )
{
	GtkWidget *ToggleElement = gtk_ui_manager_get_widget( uiManager, "/MenuBar/ViewMenuAction/ViewSectionsAction" );
//ForGTK3	gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
	gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
}
void SetToggleMenuForEditorWindow( gboolean OpenedWin )
{
	GtkWidget *ToggleElement = gtk_ui_manager_get_widget( uiManager, "/MenuBar/ViewMenuAction/ViewEditorAction" );
//ForGTK3	gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
	gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
}
void SetToggleMenuForSymbolsWindow( gboolean OpenedWin )
{
	GtkWidget *ToggleElement = gtk_ui_manager_get_widget( uiManager, "/MenuBar/ViewMenuAction/ViewSymbolsAction" );
//ForGTK3	gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
//printf("%s set toggle element %d\n", __FUNCTION__, OpenedWin );
	gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
}
void SetToggleMenuForBoolVarsWindow( gboolean OpenedWin )
{
	GtkWidget *ToggleElement = gtk_ui_manager_get_widget( uiManager, "/MenuBar/ViewMenuAction/ViewBoolVarsAction" );
//ForGTK3	gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
	gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
}
void SetToggleMenuForFreeVarsWindow( gboolean OpenedWin )
{
	GtkWidget *ToggleElement = gtk_ui_manager_get_widget( uiManager, "/MenuBar/ViewMenuAction/ViewFreeVarsAction" );
//ForGTK3	gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
	gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
}
void SetToggleMenuForLogWindow( gboolean OpenedWin )
{
	GtkWidget *ToggleElement = gtk_ui_manager_get_widget( uiManager, "/MenuBar/ViewMenuAction/ViewLogAction" );
//ForGTK3	gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
	gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
}

//toggle function ourself, depends on "status running/stopped..." 
void SetMenuStateForRunStopSwitch( gboolean Running )
{
	GtkWidget *Element = gtk_ui_manager_get_widget( uiManager, "/MenuBar/PLCAction/RunStopAction" );
	gtk_menu_item_set_label( GTK_MENU_ITEM(Element), Running?"Stop":"Run" );
	
	Element = gtk_ui_manager_get_widget( uiManager, "/ToolBar/RunStopAction" );
	gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(Element), Running?GTK_STOCK_STOP:GTK_STOCK_EXECUTE );
	gtk_tool_button_set_label( GTK_TOOL_BUTTON(Element), Running?"Stop":"Run" );
	gtk_tool_item_set_tooltip_text( GTK_TOOL_ITEM(Element), Running?"Stop logic":"Run logic" );
}
