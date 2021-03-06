#include <windows.h>

#include <gl/glui.h>
#include <GL/glaux.h>
#include <GL/gl.h>
#include <GL/glu.h>

void init ();
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
int full_screen = 0;

//  pointer to the GLUI window
GLUI * glui_window;

//  Declare live variables (related to GLUI)
int wireframe = 1;			//  Related to Wireframe Check Box
int draw = 1;				//  Related to Draw Check Box
int listbox_item_id = 12;	//  Id of the selected item in the list box
int radiogroup_item_id = 0; //  Id of the selcted radio button
float rotation_matrix[16] = { 1.0, 0.0, 0.0, 0.0, 
								0.0, 1.0, 0.0, 0.0,
								0.0, 0.0, 1.0, 0.0, 
								0.0, 0.0, 0.0, 1.0 };	
float translate_x = 0;		
float translate_y = 0;										
float translate_z = 0;		
float scale = 0.2;			//  Spinner Scale Live Variable

int no_polygons = 0;
int verteces = 3;
int point_x = 0;
int point_y = 0;
int point_z = 0;

// an array of RGB components
float color[] = { 1.0, 1.0, 1.0 };

bool chosen = false;
int polygonIndex = 0;
int vertecesIndex = 0;
int e = 0;

//coordinate x, y, z of every point
struct coordinates {
	int x;
	int y;
	int z;
};

//every polygon has 3 points with 3 coordinates x, y, z
struct polig {
	coordinates vertex[3];
}polygons[300];

//for every edge
coordinates edges[300][2];

//  Set up the GLUI window and its components
void setupGLUI ();

//  Idle callack function
void idle ();
void resetCoordinates ();
//  Declare callbacks related to GLUI
void glui_callback (int arg);

//  Declare the IDs of controls generating callbacks
enum
{
	COLOR_LISTBOX = 0,
	OBJECTYPE_RADIOGROUP,
	NO_POLYGONS,
	VERTECES=3,
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
void turnOnTheLight(void);
bool checkIfCollinearPoints(polig polygon);
bool checkIfSameCoordinatesForAllVerteces(polig polygon);
bool checkIfCommonEdge (polig polygon);

void turnOnTheLight(void){

    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };//intensitatea
	//componentei ambientale din sursa 0 este nula
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };//intensitatea
	//componentei difuze din sursa 0 este maxima pentru fiecrae compopnenta de
	//culoare
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };//intensitatea
	//componentei speculare din sursa 0 este maxima pentru fiecrae compopnenta de
	//culoare
/*  pozitia sursei de lumin� nu este cea implicit� */
	
    GLfloat light_position[] = { 1.0, 0.0, 0.0, 0.0 };
	//sursa de lumina pe axa x la infinit
    GLfloat global_ambient[] = { 0.75, 0.75, 0.75, 1.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	//lumina ambientala in scena
    
    glFrontFace (GL_CW);//orientarea implicit[ a v�rfurilor �n sensul rotirii acelor
    glEnable(GL_LIGHTING); //activare iluminare
    glEnable(GL_LIGHT0); //activare sursa 0
    glEnable(GL_AUTO_NORMAL); //activare calculare normale daca v�rfurile 
	//s-au determinat cu GL_MAP2_VERTEX_3 sau GL_MAP2_VERTEX_4
    glEnable(GL_NORMALIZE); //activare normalizare (vectori unitari) vectori
    glDepthFunc(GL_LESS); //compara�ia la ascunderea suprafe�elor
    glEnable(GL_DEPTH_TEST); //activare test ad�ncime
}

void init ()
{	
	//  Set the frame buffer clear color to black. 
	glClearColor (0.0, 0.0, 0.0, 0.0);
	turnOnTheLight();
}

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

void reshape (int w, int h)
{
	//  Stay updated with the window width and height
	window_width = w;
	window_height = h;

	//  Reset viewport
	glViewport(0, 0, window_width, window_height);
}

GLUI_EditText *edittext_polygons;
GLUI_EditText *edittext_verteces;

void setupGLUI ()
{
	//  Set idle function
	GLUI_Master.set_glutIdleFunc (idle);
	
	//  Create GLUI window //apar ferestrele unde apare desenul si options mai apropiate
	glui_window = GLUI_Master.create_glui ("Modelare Suprafete Poligonale", 0, window_x - 235, window_y);

	// 'Polygon' Panel
	GLUI_Panel *polygon_panel = glui_window->add_panel ("Polygon");
	edittext_polygons = glui_window->add_edittext_to_panel(polygon_panel, "Number of polygons: ", GLUI_EDITTEXT_INT, &no_polygons, NO_POLYGONS, glui_callback);
	edittext_polygons->set_int_limits(1, 100);
	//  Add separator
	glui_window->add_separator_to_panel (polygon_panel);
	GLUI_Panel *vertex_panel = glui_window->add_panel_to_panel (polygon_panel, "Verteces");
	edittext_verteces = glui_window->add_edittext_to_panel(vertex_panel, "Number of verteces per polygon: ", GLUI_EDITTEXT_INT, &verteces, VERTECES, glui_callback);
	edittext_verteces->disable();

	GLUI_Panel *points_panel = glui_window->add_panel_to_panel (vertex_panel, "Coordinates");
	GLUI_EditText *edittext_x = glui_window->add_edittext_to_panel(points_panel, "x", GLUI_EDITTEXT_INT, &point_x, POINT_X, glui_callback);
	GLUI_EditText *edittext_y = glui_window->add_edittext_to_panel(points_panel, "y", GLUI_EDITTEXT_INT, &point_y, POINT_Y, glui_callback);
	GLUI_EditText *edittext_z = glui_window->add_edittext_to_panel(points_panel, "z", GLUI_EDITTEXT_INT, &point_z, POINT_Z, glui_callback);
	GLUI_Button *ok_button = glui_window->add_button_to_panel (points_panel, "Ok", OK_BUTTON, glui_callback);	
	GLUI_Button *reset_button = glui_window->add_button_to_panel (polygon_panel, "Reset", RESET_BUTTON, glui_callback);	
	
	// 'Drawing Properties' Panel
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

	// 'Transformation' Panel
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

	// 'Quit' Button
	//  Add the Quit Button
	glui_window->add_button ("Quit", QUIT_BUTTON, glui_callback);
	//  Let the GLUI window know where its main graphics window is
	glui_window->set_main_gfx_window( main_window );
}

void resetCoordinates() {
	for(int i = 0; i < no_polygons; i++) {
		for(int j = 0; j < 3; j++) {
			polygons[i].vertex[j].x = NULL; 
			polygons[i].vertex[j].y = NULL; 
			polygons[i].vertex[j].z = NULL; 
		}		
	}

	for(int k = 0; k < e; k++) {
		edges[k][0].x = NULL;
		edges[k][0].y = NULL;
		edges[k][0].z = NULL;
		edges[k][1].x = NULL;
		edges[k][1].y = NULL;
		edges[k][1].z = NULL;
	}

	no_polygons = 0;
	polygonIndex = 0;
	vertecesIndex = 0;
	e = 0;
}

//determinantul este 0
bool checkIfCollinearPoints(polig polygon)
{
	int expression;
	expression = (polygon.vertex[0].x * polygon.vertex[1].y * polygon.vertex[2].z) + (polygon.vertex[2].x * polygon.vertex[0].y * polygon.vertex[1].z);
	expression = expression + (polygon.vertex[1].x * polygon.vertex[2].y * polygon.vertex[0].z) - (polygon.vertex[2].x * polygon.vertex[1].y * polygon.vertex[0].z);
	expression = expression - (polygon.vertex[0].x * polygon.vertex[2].y * polygon.vertex[1].z) - (polygon.vertex[1].x * polygon.vertex[0].y * polygon.vertex[2].z);

	return expression == 0;
}

bool checkIfSameCoordinatesForAllVerteces(polig polygon) 
{
	bool x_coord = (polygon.vertex[0].x == polygon.vertex[1].x) && (polygon.vertex[0].x == polygon.vertex[2].x) && (polygon.vertex[1].x == polygon.vertex[2].x);
	bool y_coord = (polygon.vertex[0].y == polygon.vertex[1].y) && (polygon.vertex[0].y == polygon.vertex[2].y) && (polygon.vertex[1].y == polygon.vertex[2].y);
	bool z_coord = (polygon.vertex[0].z == polygon.vertex[1].z) && (polygon.vertex[0].z == polygon.vertex[2].z) && (polygon.vertex[1].z == polygon.vertex[2].z);

	return x_coord && y_coord && z_coord;
}

void addEdge(polig polygon) 
{
		edges[e][0] = polygon.vertex[0];
		edges[e][1] = polygon.vertex[1];
		e++;
		edges[e][0] = polygon.vertex[0];
		edges[e][1] = polygon.vertex[2];
		e++;
		edges[e][0] = polygon.vertex[1];
		edges[e][1] = polygon.vertex[2];	
		e++;
		/*	
		for(int g =0; g < e; g++){
			printf("edge:  X: %d, Y: %d, Z: %d   ", edges[g][0].x, edges[g][0].y, edges[g][0].z);
			printf("edge:  X: %d, Y: %d, Z: %d\n", edges[g][1].x, edges[g][1].y, edges[g][1].z);
		}
		*/
}

bool checkIfCommonEdge (polig polygon) 
{
	int i;
	bool edgeFound = false;
	
	for(i = 0; i < e; i++) {
			bool expr1 = ((polygon.vertex[0].x == edges[i][0].x) &&  (polygon.vertex[0].y == edges[i][0].y) &&  (polygon.vertex[0].z == edges[i][0].z));
			bool expr2 = ((polygon.vertex[0].x == edges[i][1].x) &&  (polygon.vertex[0].y == edges[i][1].y) &&  (polygon.vertex[0].z == edges[i][1].z));

			bool expr3 = ((polygon.vertex[1].x == edges[i][0].x) &&  (polygon.vertex[1].y == edges[i][0].y) &&  (polygon.vertex[1].z == edges[i][0].z));
			bool expr4 = ((polygon.vertex[1].x == edges[i][1].x) &&  (polygon.vertex[1].y == edges[i][1].y) &&  (polygon.vertex[1].z == edges[i][1].z));

			bool expr5 = ((polygon.vertex[2].x == edges[i][0].x) &&  (polygon.vertex[2].y == edges[i][0].y) &&  (polygon.vertex[2].z == edges[i][0].z));
			bool expr6 = ((polygon.vertex[2].x == edges[i][1].x) &&  (polygon.vertex[2].y == edges[i][1].y) &&  (polygon.vertex[2].z == edges[i][1].z));

			bool expr7a = ((expr1 && expr4));
			bool expr7b = ((expr1 && expr6));

			bool expr8a = ((expr3 && expr1));
			bool expr8b = ((expr3 && expr6));

			bool expr9a = ((expr5 && expr2));
			bool expr9b = ((expr5 && expr4));

			bool expr10a = ((expr2 && expr3));
			bool expr10b = ((expr2 && expr5));

			bool expr11a = ((expr4 && expr1));
			bool expr11b = ((expr4 && expr5));

			bool expr12a = ((expr6 && expr1));
			bool expr12b = ((expr6 && expr3));

			if(expr7a || expr8a || expr9a || expr7b || expr8b || expr9b || expr10a || expr11a || expr12a || expr10b || expr11b || expr12b) {
				edgeFound = true;			
			}
	}

	if(edgeFound) {
		return true;
	}
	return false;
}

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

		case NO_POLYGONS:
			printf ("Number of polygons: %d\n", no_polygons);
			chosen = true;
		break;

		case OK_BUTTON:
			if(polygonIndex < no_polygons) {

				if(polygonIndex == 0) {
					polygons[polygonIndex].vertex[vertecesIndex].x = point_x;
					polygons[polygonIndex].vertex[vertecesIndex].y = point_y;
					polygons[polygonIndex].vertex[vertecesIndex].z = point_z;
					printf("Polygon number: %d, vertex %d = X: %d, Y: %d, Z: %d\n", polygonIndex, vertecesIndex, point_x, point_y, point_z);
			
					vertecesIndex++;

					if(vertecesIndex == 3 ) {
						vertecesIndex = 0;
						if(checkIfSameCoordinatesForAllVerteces(polygons[polygonIndex])) {
							printf("All verteces have the same coordinates; please insert other coordinates\n");
							break;
						}
						addEdge(polygons[polygonIndex]);
						polygonIndex++;
					}
				}
			else {
			
					polygons[polygonIndex].vertex[vertecesIndex].x = point_x;
					polygons[polygonIndex].vertex[vertecesIndex].y = point_y;
					polygons[polygonIndex].vertex[vertecesIndex].z = point_z;
					printf("Polygon number: %d, vertex %d = X: %d, Y: %d, Z: %d\n", polygonIndex, vertecesIndex, point_x, point_y, point_z);
			
					vertecesIndex++;		
					if(vertecesIndex == 3 ) {
						vertecesIndex = 0;
						if(checkIfSameCoordinatesForAllVerteces(polygons[polygonIndex])) {
							printf("All verteces have the same coordinates; please insert other coordinates\n");
							break;
						}
						if(checkIfCommonEdge(polygons[polygonIndex])) {
							addEdge(polygons[polygonIndex]);	
							polygonIndex++;
						}
						else {
							printf("This shape is incorrect; please insert other coordinates\n");
							for (int i = 0; i < 3; i++) {
								polygons[polygonIndex].vertex[i].x = 0;
								polygons[polygonIndex].vertex[i].y = 0;
								polygons[polygonIndex].vertex[i].z = 0;
							}
						}
					}			
				} 
			}
		break;

		case RESET_BUTTON:	
			chosen = false;
			edittext_polygons->enable();
			resetCoordinates();
			glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		break;

		case TRANSLATION_X:
			printf ("Translating X: ");
			printf ("X: %f, Y: %f.\n", translate_x, translate_x);
			break;

		case TRANSLATION_Y:
			printf ("Translating X: ");
			printf ("X: %f, Y: %f.\n", translate_y, translate_y);
			break;

		case TRANSLATION_Z:
			printf ("Translating Z coordinate: ");
			printf ("Z: %f.\n", translate_z);
			break;

		case ROTATION:
			printf ("Rotating archball!\n");
			printMatrixf (rotation_matrix);
		break;

		case SCALE_SPINNER:
			printf ("Scaling Object: %f.\n", scale);
		break;

		case QUIT_BUTTON:		
			printf ("Exit!\n");
			exit (1);
		break;

	}
}

void display (void)
{	
    GLfloat more_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
	//  Clear the window 
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();

    glMaterialfv(GL_FRONT, GL_AMBIENT, more_ambient);
    glMaterialf(GL_FRONT, GL_SHININESS, 100.0);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, color);

	drawObject();
	//  Swap contents of backward and forward frame buffers
	glutSwapBuffers (); 
}

void drawObject ()
{	
	//  Draw the object only if the Draw check box is selected
	if (draw)
	{
		if (chosen == true) {
			edittext_polygons->disable();
		}

		glPushMatrix ();	
		glTranslatef (translate_x, translate_y, -translate_z);
		//  Apply the rotation matrix
		glMultMatrixf (rotation_matrix);
		glScalef (scale, scale, scale);

		int i, j;
		if(wireframe) {
				for(j = 0; j < no_polygons; j++) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glBegin(GL_TRIANGLES);  				
					for(i = 0; i < verteces; i++){
						glVertex3f(polygons[j].vertex[i].x, polygons[j].vertex[i].y, polygons[j].vertex[i].z);
					}				
					glEnd();
				}
		}
		else {
				for(j = 0; j < no_polygons; j++){
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glBegin(GL_TRIANGLES);  				
					for(i = 0; i < verteces; i++) {
						glVertex3f(polygons[j].vertex[i].x, polygons[j].vertex[i].y, polygons[j].vertex[i].z);
					}				
					glEnd();
				}
		}
        //  Pop our matrix from the model view stack after we finish drawing
		glPopMatrix ();
	}
}

void idle ()
{
	glutSetWindow (main_window);
	glutPostRedisplay ();
	Sleep (50);
}

void main (int argc, char **argv)
{
	centerOnScreen ();

	glutInit(&argc, argv);
	glutInitWindowSize (window_width, window_height);
	glutInitWindowPosition (window_x, window_y);
	glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	main_window = glutCreateWindow (window_title);

	if (full_screen)
		glutFullScreen ();

	init();
	glutDisplayFunc (display);
	glutReshapeFunc  (reshape);
	setupGLUI ();
	glutMainLoop();	
}