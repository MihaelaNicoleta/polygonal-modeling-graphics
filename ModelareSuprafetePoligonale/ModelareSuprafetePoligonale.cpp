//  Include windows library in order to use the Sleep function
#include <windows.h>

//  Include GLUI, GLUT, OpenGL, and GLU libraries
#include <gl/glui.h>
#include <GL/glaux.h>
#include <GL/gl.h>
#include <GL/glu.h>

//*************************************************************************
//  GLUT Declarations
//*************************************************************************

//  Initialization
void init ();
bool checkIfCommonLine();

//  Callback functions
void display (void);
void reshape (int w, int h);
void pmotion (int x, int y);
void drawCube(void);

//  define the window position on screen
int window_x;
int window_y;

//  variables representing the window size
int window_width = 512;
int window_height = 512;

//  variable representing the window title
char *window_title = "Suprafete Poligonale";

//  The id of the main window
GLuint main_window;

//  Tells whether to display the window full screen or not
//  Press Alt + Esc to exit a full screen.
int full_screen = 0;

bool chosen = false;
bool chosenV = false;

//coordinates index
int index = 0;  //replaced with polygon index

int polygonIndex = 0;
int vertecesIndex = 0;

//*************************************************************************
//  GLUI Declarations
//*************************************************************************

//  pointer to the GLUI window
GLUI * glui_window;

//  Declare live variables (related to GLUI)
int wireframe = 1;			//  Related to Wireframe Check Box
int draw = 1;				//  Related to Draw Check Box
int listbox_item_id = 12;	//  Id of the selected item in the list box
int radiogroup_item_id = 0; //  Id of the selcted radio button
float rotation_matrix[16]	//  Rotation Matrix Live Variable Array
							= { 1.0, 0.0, 0.0, 0.0, 
								0.0, 1.0, 0.0, 0.0,
								0.0, 0.0, 1.0, 0.0, 
								0.0, 0.0, 0.0, 1.0 };	
float translate_x = 0;		//  Translation XY Live Variable
float translate_y = 0;										
float translate_z = 0;		//  Translation Z Live Variable
float scale = 0.2;			//  Spinner Scale Live Variable

int no_polygons = 0;
int verteces = 3;
int point_x = 0;
int point_y = 0;
int point_z = 0;

struct coordinates {
	int x;
	int y;
	int z;
}points[10];

/*
struct coordinates {
	int x;
	int y;
	int z;
};
*/
//every polygon has 3 points with 3 coordinates x, y, z
struct polig {
	coordinates verteces[3];
}polygons[50];

//keeps every polygons' vertex
int vertecesMatrix[50][3];

// an array of RGB components
float color[] = { 1.0, 1.0, 1.0 };

//  Set up the GLUI window and its components
void setupGLUI ();

//  Idle callack function
void idle ();
void resetCoordinates ();
//  Declare callbacks related to GLUI
void glui_callback (int arg);
void turnOnTheLight(void);

//  Declare the IDs of controls generating callbacks
enum
{
	COLOR_LISTBOX = 0,
	OBJECTYPE_RADIOGROUP,
	NO_POLYGONS,
	VERTECES,
	POINT_X,
	POINT_Y,
	POINT_Z,
	TRANSLATION_X,
	TRANSLATION_Y,
	TRANSLATION_Z,
	OK_BUTTON,
	RESET_BUTTON,
	ROTATION,
	SCALE_SPINNER,
	QUIT_BUTTON
};
//  Support Functions
void centerOnScreen ();
void drawObject ();
void printMatrixf (float *matrix);

//*************************************************************************
//  GLUT Functions.
//*************************************************************************

void turnOnTheLight(void){

    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };//intensitatea
	//componentei ambientale din sursa 0 este nula
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };//intensitatea
	//componentei difuze din sursa 0 este maxima pentru fiecrae compopnenta de
	//culoare
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };//intensitatea
	//componentei speculare din sursa 0 este maxima pentru fiecrae compopnenta de
	//culoare
/*  pozitia sursei de luminã nu este cea implicitã */
	
    GLfloat light_position[] = { 1.0, 0.0, 0.0, 0.0 };
	//sursa de lumina pe axa x la infinit
    GLfloat global_ambient[] = { 0.75, 0.75, 0.75, 1.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	//lumina ambientala in scena
    
    glFrontFace (GL_CW);//orientarea implicit[ a vârfurilor în sensul rotirii acelor
    glEnable(GL_LIGHTING); //activare iluminare
    glEnable(GL_LIGHT0); //activare sursa 0
    glEnable(GL_AUTO_NORMAL); //activare calculare normale daca vârfurile 
	//s-au determinat cu GL_MAP2_VERTEX_3 sau GL_MAP2_VERTEX_4
    glEnable(GL_NORMALIZE); //activare normalizare (vectori unitari) vectori
    glDepthFunc(GL_LESS); //comparaþia la ascunderea suprafeþelor
    glEnable(GL_DEPTH_TEST); //activare test adâncime

}

//-------------------------------------------------------------------------
//  Set OpenGL program initial state.
//-------------------------------------------------------------------------
void init ()
{	
	//  Set the frame buffer clear color to black. 
	glClearColor (0.0, 0.0, 0.0, 0.0);
	turnOnTheLight();
}

//-------------------------------------------------------------------------
//  This function sets the window x and y coordinates
//  such that the window becomes centered
//-------------------------------------------------------------------------
void centerOnScreen ()
{
	window_x = (glutGet (GLUT_SCREEN_WIDTH) - window_width)/2;
	window_y = (glutGet (GLUT_SCREEN_HEIGHT) - window_height)/2;
}

//-------------------------------------------------------------------------
//  Function to print matrix on command prompt.
//-------------------------------------------------------------------------
void printMatrixf (float *matrix)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			printf ("%f%\t", matrix[i*4 + j]);
		}

		printf ("\n");
	}
}

//-------------------------------------------------------------------------
//  This function is passed to the glutReshapeFunc and is called 
//  whenever the window is resized.
//-------------------------------------------------------------------------
void reshape (int w, int h)
{
	//  Stay updated with the window width and height
	window_width = w;
	window_height = h;

	//  Reset viewport
	glViewport(0, 0, window_width, window_height);
}

void resetCoordinates() {
	/*
		for(int i = 0; i < 10; i++) {
			points[i].x = 0;
			points[i].y = 0;
			points[i].z = 0;
		}
		*/
}
//-------------------------------------------------------------------------
//  This function is passed to the glutKeyboardFunc and is called 
//  whenever the user hits a key.
//-------------------------------------------------------------------------
void keyboard (unsigned char key, int x, int y)
{	
	if(key == 27) 
	{
		exit(1);
	}
}

GLUI_EditText *edittext_polygons;
GLUI_EditText *edittext_verteces;

void setupGLUI ()
{
	//  Set idle function
	GLUI_Master.set_glutIdleFunc (idle);
	
	//  Create GLUI window //apar ferestrele unde apare desenul si options mai apropiate
	glui_window = GLUI_Master.create_glui ("Modelare Suprafete Poligonale", 0, window_x - 235, window_y);

	//---------------------------------------------------------------------
	// 'Polygon' Panel
	//---------------------------------------------------------------------
	GLUI_Panel *polygon_panel = glui_window->add_panel ("Polygon");
	edittext_polygons = glui_window->add_edittext_to_panel(polygon_panel, "Number of polygons: ", GLUI_EDITTEXT_INT, &no_polygons, NO_POLYGONS, glui_callback);
	edittext_polygons->set_int_limits(1, 10);
		//  Add separator
	glui_window->add_separator_to_panel (polygon_panel);
	GLUI_Panel *vertex_panel = glui_window->add_panel_to_panel (polygon_panel, "Verteces");
	edittext_verteces = glui_window->add_edittext_to_panel(vertex_panel, "Number of verteces per polygon: ", GLUI_EDITTEXT_INT, &verteces, VERTECES, glui_callback);
	edittext_verteces->set_int_limits(3, 10);

	GLUI_Panel *points_panel = glui_window->add_panel_to_panel (vertex_panel, "Coordinates");

	GLUI_EditText *edittext_x = glui_window->add_edittext_to_panel(points_panel, "x", GLUI_EDITTEXT_INT, &point_x, POINT_X, glui_callback);
	//point_x = edittext_x->get_int_val();

	GLUI_EditText *edittext_y = glui_window->add_edittext_to_panel(points_panel, "y", GLUI_EDITTEXT_INT, &point_y, POINT_Y, glui_callback);
	//point_y = edittext_y->get_int_val();

	GLUI_EditText *edittext_z = glui_window->add_edittext_to_panel(points_panel, "z", GLUI_EDITTEXT_INT, &point_z, POINT_Z, glui_callback);
	//point_z = edittext_z->get_int_val();

	GLUI_Button *ok_button = glui_window->add_button_to_panel (points_panel, "Ok", OK_BUTTON, glui_callback);	

	GLUI_Button *reset_button = glui_window->add_button_to_panel (polygon_panel, "Reset", RESET_BUTTON, glui_callback);	


	//---------------------------------------------------------------------
	// 'Drawing Properties' Panel
	//---------------------------------------------------------------------

	//  Add the 'Object Properties' Panel to the GLUI window
	GLUI_Panel *op_panel = glui_window->add_panel ("Drawing Properties");

	//  Add the Draw Check box to the 'Object Properties' Panel
	glui_window->add_checkbox_to_panel (op_panel, "Draw", &draw );

	//  Add the Wireframe Check box to the 'Object Properties' Panel
	glui_window->add_checkbox_to_panel (op_panel, "Wireframe", &wireframe );

	//  Add a separator
	glui_window->add_separator_to_panel (op_panel);

	//  Add the Color listbox to the 'Object Properties' Panel
	GLUI_Listbox *color_listbox = glui_window->add_listbox_to_panel (op_panel, 
									"Color", &listbox_item_id, COLOR_LISTBOX, glui_callback);
	
	//  Add the items to the listbox
	color_listbox->add_item (1, "Black");
	color_listbox->add_item (2, "Blue");
	color_listbox->add_item (3, "Cyan");
	color_listbox->add_item (4, "Dark Grey");
	color_listbox->add_item (5, "Grey");
	color_listbox->add_item (6, "Green");
	color_listbox->add_item (7, "Light Grey");
	color_listbox->add_item (8, "Magenta");
	color_listbox->add_item (9, "Orange");
	color_listbox->add_item (10, "Pink");
	color_listbox->add_item (11, "Red");
	color_listbox->add_item (12, "White");
	color_listbox->add_item (13, "Yellow");

	//  Select the White Color by default
	color_listbox->set_int_val (12);

	//---------------------------------------------------------------------
	// 'Transformation' Panel
	//---------------------------------------------------------------------

	//  Add the 'Transformation' Panel to the GLUI window
	GLUI_Panel *transformation_panel = glui_window->add_panel ("Transformation");

	//  Create transformation panel 1 that will contain the Translation controls
	GLUI_Panel *transformation_panel1 = glui_window->add_panel_to_panel (transformation_panel, "");

	//  Add the xy translation control
	GLUI_Translation *translation_x = glui_window->add_translation_to_panel (transformation_panel1, "Translation X", GLUI_TRANSLATION_X, &translate_x, TRANSLATION_X, glui_callback );
	
	//  Set the translation speed
	translation_x->set_speed( 0.005 );

		//  Add column, but don't draw it
	glui_window->add_column_to_panel (transformation_panel1, false);

	GLUI_Translation *translation_y = glui_window->add_translation_to_panel (transformation_panel1, "Translation Y", GLUI_TRANSLATION_Y, &translate_y, TRANSLATION_Y, glui_callback );
	translation_y->set_speed( 0.005 );

	//  Add column, but don't draw it
	glui_window->add_column_to_panel (transformation_panel1, false);

	//  Add the z translation control
	GLUI_Translation *translation_z = glui_window->add_translation_to_panel (transformation_panel1, "Translation Z", GLUI_TRANSLATION_Z, &translate_z, TRANSLATION_Z, glui_callback );

	//  Set the translation speed
	translation_z->set_speed( 0.005 );

	//  Create transformation panel 2 that will contain the rotation and spinner controls
	GLUI_Panel *transformation_panel2 = glui_window->add_panel_to_panel (transformation_panel, "");

	//  Add the rotation control
	glui_window->add_rotation_to_panel (transformation_panel2, "Rotation", rotation_matrix, ROTATION, glui_callback);
	
	//  Add separator
	glui_window->add_separator_to_panel (transformation_panel2);

	//  Add the scale spinner
	GLUI_Spinner *spinner = glui_window->add_spinner_to_panel (transformation_panel2, "Scale", GLUI_SPINNER_FLOAT, &scale, SCALE_SPINNER, glui_callback);
	
	//  Set the limits for the spinner
	spinner->set_float_limits ( -4.0, 4.0 );

	//---------------------------------------------------------------------
	// 'Quit' Button
	//---------------------------------------------------------------------

	//  Add the Quit Button
	glui_window->add_button ("Quit", QUIT_BUTTON, glui_callback);

	//  Let the GLUI window know where its main graphics window is
	glui_window->set_main_gfx_window( main_window );
}

//-------------------------------------------------------------------------
//  GLUI callback function.
//-------------------------------------------------------------------------
void glui_callback (int control_id)
{
	//  Behave based on control ID
	switch (control_id)
	{
		//  Color Listbox item changed
		case COLOR_LISTBOX:

			printf ("Color List box item changed: ");

			switch (listbox_item_id)
			{
				//  Select black color
				case 1:
					color[0] = 0/255.0;
					color[1] = 0/255.0;
					color[2] = 0/255.0;
					break;
				//  Select blue color
				case 2:
					color[0] = 0/255.0;
					color[1] = 0/255.0;
					color[2] = 255/255.0;
					break;
				//  Select cyan color
				case 3:	
					color[0] = 0/255.0;
					color[1] = 255/255.0;
					color[2] = 255/255.0;
					break;
				//  Select dark grey color
				case 4:
					color[0] = 64/255.0;
					color[1] = 64/255.0;
					color[2] = 64/255.0;
					break;
				//  Select grey color
				case 5:
					color[0] = 128/255.0;
					color[1] = 128/255.0;
					color[2] = 128/255.0;
					break;
				//  Select green color
				case 6:
					color[0] = 0/255.0;
					color[1] = 255/255.0;
					color[2] = 0/255.0;
					break;
				//  Select light gray color
				case 7:
					color[0] = 192/255.0;
					color[1] = 192/255.0;
					color[2] = 192/255.0;
					break;
				//  Select magenta color
				case 8:
					color[0] = 192/255.0;
					color[1] = 64/255.0;
					color[2] = 192/255.0;
					break;
				//  Select orange color
				case 9:
					color[0] = 255/255.0;
					color[1] = 192/255.0;
					color[2] = 64/255.0;
					break;
				//  Select pink color
				case 10:
					color[0] = 255/255.0;
					color[1] = 0/255.0;
					color[2] = 255/255.0;
					break;
				//  Select red color
				case 11:
					color[0] = 255/255.0;
					color[1] = 0/255.0;
					color[2] = 0/255.0;
					break;
				//  Select white color
				case 12:
					color[0] = 255/255.0;
					color[1] = 255/255.0;
					color[2] = 255/255.0;
					break;
				//  Select yellow color
				case 13:
					color[0] = 255/255.0;
					color[1] = 255/255.0;
					color[2] = 0/255.0;
					break;
			}

			printf ("Item %d selected.\n", listbox_item_id);

		break;
		
		case VERTECES:

			printf ("Number of points/polygon: %d\n", verteces);
			chosenV = true;

		break;

		case NO_POLYGONS:
			printf ("Number of polygons: %d\n", no_polygons);
			chosen = true;
		break;

			//  Ok Button clicked
		case OK_BUTTON:
			
			printf ("X: %d, Y: %d, Z: %d\n", point_x, point_y, point_z );
			/*polygons[polygonIndex].verteces[vertecesIndex].x = point_x;
			polygons[polygonIndex].verteces[vertecesIndex].y = point_y;
			polygons[polygonIndex].verteces[vertecesIndex].z = point_z;
			
			polygonIndex++;
			vertecesIndex++;

			if(vertecesIndex >=3 ) {
				printf ("enough verteces");
				vertecesIndex = 0;
			} */
			points[index].x = point_x;
			points[index].y = point_y;
			points[index].z = point_z;
			index++;
			
		break;

		case RESET_BUTTON:			
			chosen = false;
			edittext_polygons->enable();
			chosenV = false;
			edittext_verteces->enable();
			resetCoordinates();
			//index = 0;
			polygonIndex = 0;
			vertecesIndex = 0;
		break;

		//  Translation XY control
		case TRANSLATION_X:

			printf ("Translating X: ");
			printf ("X: %f, Y: %f.\n", translate_x, translate_x);

			break;

		case TRANSLATION_Y:

			printf ("Translating X: ");
			printf ("X: %f, Y: %f.\n", translate_y, translate_y);

			break;

		//  Translation Z control
		case TRANSLATION_Z:

			printf ("Translating Z coordinate: ");
			printf ("Z: %f.\n", translate_z);

			break;

		//  rotation control is being rotated
		case ROTATION:

			printf ("Rotating archball!\n");
			printMatrixf (rotation_matrix);

		break;

		//  Scaling
		case SCALE_SPINNER:

			printf ("Scaling Object: %f.\n", scale);

		break;

		//  Quit Button clicked
		case QUIT_BUTTON:
		
			printf ("Exit!\n");

			exit (1);

		break;

	}
}

//-------------------------------------------------------------------------
//  This function is passed to glutDisplayFunc in order to display 
//	OpenGL contents on the window.
//-------------------------------------------------------------------------
void display (void)
{	
    GLfloat low_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat more_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
    GLfloat most_ambient[] = { 1.0, 1.0, 1.0, 1.0 };

	//  Clear the window 
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	 glLoadIdentity ();
	//  Set the color
	//glColor3fv (color);

    /*  material has small ambient reflection   */
	//coeficient de reflexie pentru lumina ambientala mic
    glMaterialfv(GL_FRONT, GL_AMBIENT, more_ambient);
    glMaterialf(GL_FRONT, GL_SHININESS, 100.0);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
	drawObject();

	//  Swap contents of backward and forward frame buffers
	glutSwapBuffers (); 

}

//-------------------------------------------------------------------------
//  Draws our object.
//-------------------------------------------------------------------------
void drawObject ()
{
	//  Draw the object only if the Draw check box is selected
	if (draw)
	{
		if (chosen == true) {
			edittext_polygons->disable();
			//chosen = false;
		}
		if (chosenV == true) {
			edittext_verteces->disable();
			//chosenV = false;
		}

		glPushMatrix ();	
		//  Apply the translation
		glTranslatef (translate_x, translate_y, -translate_z);
		//  Apply the rotation matrix
		glMultMatrixf (rotation_matrix);
		//  Apply the scaling
		glScalef (scale, scale, scale);

		if(wireframe) {
			int i, j, k = 0;

				for(j = 0; j < no_polygons; j++){
					glBegin(GL_POLYGON);  				
					for(i = 0; i < verteces; i++){
					 glVertex3f(points[k].x, points[k].y, points[k].z);
						//glVertex3f(polygons[j].verteces[k].x, polygons[j].verteces[k].y, polygons[j].verteces[k].z);
						k++;
			}				
					glEnd();
		}
		}
		else {
			auxSolidTeapot(0.5);
		}

        //  Pop our matrix from the model view stack after we finish drawing
		glPopMatrix ();
	}
}

//-------------------------------------------------------------------------
//  Idle Callback function.
//
//  Set the main_window as the current window to avoid sending the
//  redisplay to the GLUI window rather than the GLUT window. 
//  Call the Sleep function to stop the GLUI program from causing
//  starvation.
//-------------------------------------------------------------------------
void idle ()
{
	glutSetWindow (main_window);
	glutPostRedisplay ();
	Sleep (50);
}

//*************************************************************************
//  Program Main method.
//*************************************************************************
void main (int argc, char **argv)
{
	//  Set the window x and y coordinates such that the 
	//  window becomes centered
	centerOnScreen ();

	//  Connect to the windowing system + create a window
	//  with the specified dimensions and position
	//  + set the display mode + specify the window title.
	glutInit(&argc, argv);
	glutInitWindowSize (window_width, window_height);
	glutInitWindowPosition (window_x, window_y);
	glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	main_window = glutCreateWindow (window_title);

	//  View in full screen if the full_screen flag is on
	if (full_screen)
		glutFullScreen ();


	//  Set OpenGL context initial state.
	init();

	// Set the GLUT callback functions
	glutDisplayFunc (display);
	glutReshapeFunc  (reshape);
	glutKeyboardFunc (keyboard);
	//  Setup all GLUI stuff
	setupGLUI ();

	//  Start GLUT event processing loop
	glutMainLoop();
	
}