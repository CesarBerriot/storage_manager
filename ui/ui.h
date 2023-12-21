//
// Created by César B. on 12/14/2023.
//

#pragma once

#include "../luigi/luigi.h"
#include "rendering.h"
#include "callbacks.h"

//@formatter:off
extern struct g_ui_struct
{
	UIWindow 	*	window;
	UIPanel	 	*		main_panel;
	UITabPane 	*			display_modes_pane;
	UISplitPane	* 				gui_display_pane;
	UIPanel	 	* 					dirs_pane;
	UIPanel	 	* 						dir_names_panel;
	UIPanel	 	* 						dir_sizes_panel;
	UISplitPane	* 					files_pane;
	UIPanel	 	* 						file_names_panel;
	UIPanel	 	* 						file_sizes_panel;
	UICode	 	* 				console_code;
	UIPanel	 	* 			button_panel;
	UIButton 	* 				reload_button;
	UIButton 	* 				rewind_button;
	UIButton 	* 				enter_button;
	UIButton 	* 				next_button;
	UIButton 	* 				previous_button;
} g_ui;
//@formatter:on
