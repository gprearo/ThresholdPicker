#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

#define MAX_BINARY_VALUE 255
#define MAX_GRAY_VALUE 255

#define WINDOW_TITLE "Threshold Picker"
#define TB_LOWER_NAME "Threshold lower value"
#define TB_UPPER_NAME "Threshold upper value"

//Types of effects - this is used when writing the applied
//effects to a string
#define EROSION 3
#define DILATION 4
#define OPENING 5
#define CLOSING 6

//This is the string that hold all applied effects in order
char *applied_effects = NULL ;

using namespace cv ;


//When using a single threshold set this to 255
int threshold_upper_value = 255 ; 
int threshold_lower_value = 0 ;

//Declaration of image handlers
Mat source, modified, gray, binary ;

//Prototype of trackbar envent handler function
void trackbar_event(int , void *) ;

//Prototype of effects menu function
int effects_menu() ;

//Prototype of apply threshold function
void apply_threshold() ;

//Prototype of write effects to string function
void write_effects_str(int operation, int kernel_type, int kernel_size) ;

//Prototype of function apply dilation
void apply_dilation() ;

int main(int argc, char *argv[]) {


	if (argc != 2) {
		printf("usage: %s <filename>\n", argv[0]) ;
		exit(2) ;
	}

	//Reads the original image and converts it to gray scale
	source = imread(argv[1], CV_LOAD_IMAGE_COLOR) ;
	modified = source.clone() ;
	cvtColor(source, gray, CV_BGR2GRAY) ;


	namedWindow(WINDOW_TITLE, CV_WINDOW_AUTOSIZE) ;

	//Upper threshold value trackbar
	createTrackbar(TB_UPPER_NAME, WINDOW_TITLE, &threshold_upper_value, MAX_BINARY_VALUE, trackbar_event) ;
	createTrackbar(TB_LOWER_NAME, WINDOW_TITLE, &threshold_lower_value, MAX_BINARY_VALUE, trackbar_event) ;

	trackbar_event(0, 0) ;

	waitKey(1) ;
	//Keep showing menu until user select to exit
	while(effects_menu()) ;

	return 0 ;
}


//Apply dual side threshold on the image with effects applied
//It does not show the image on the screen!
void apply_threshold() {
	cvtColor(modified, gray, CV_BGR2GRAY) ;
	//Apply a dual side threshold on gray image to binarize it
	inRange(gray, threshold_lower_value, threshold_upper_value, binary) ;
}

//Routine to handle the trackbar value change
//It is used to show the image on screen as well
void trackbar_event(int , void *) {
	apply_threshold() ;
	imshow(WINDOW_TITLE, binary) ;
	waitKey(1) ;
}

//Clear all effects applied to the image by loading it again
//It does clear the string that hold the applied affects too
void clear_effects() {
	//Deallocate the modified image and makes a copy of the original
	modified.deallocate() ;
	modified = source.clone() ;
	//Also clears the applied effects string
	free(applied_effects) ;
	applied_effects = NULL  ;
}

//Reads the information about the kernel of the effect to be applied
//It receives two pointers to ints which will hold the type and the size of
//the kernel. The type is the same as the opencv library.
void read_kernel(int *type, int *size) {
	printf("Which type of kernel do you want?\n"
			"0 - Rectangular\n"
			"1 - Cross\n"
			"2 - Elliptcal\n") ;
	scanf("%d", type) ;

	printf("Enter the size of the kernel: ") ;
	scanf("%d", size) ;
}


//Update the string that holds all effects applied
//The operation is defined on the top of this file
//The kernel type is the same as the opencv library
void write_effects_str(int operation, int kernel_type, int kernel_size) {
	char buffer[1024] = "" ;

	switch (operation) {
		case EROSION:
			strcat(buffer, "Erosion: ") ;
			break ;
		case DILATION:
			strcat(buffer, "Dilation: ") ;
			break ;
		case OPENING:
			strcat(buffer, "Opening: ") ;
			break ;
		case CLOSING:
			strcat(buffer, "Closing: ") ;
			break ;
	}

	char kernel_type_str[20] ;
	switch (kernel_type) {
		case 0:
			strcpy(kernel_type_str, "Rectangular") ;
			break ;
		case 1:
			strcpy(kernel_type_str, "Cross") ;
			break ;
		case 2:
			strcpy(kernel_type_str, "Elliptical") ;
			break ;
	}	
	sprintf(buffer, "%sKernel(%s, %d)\n", buffer, kernel_type_str, kernel_size) ;

	int len_appl_eff = 0 ;
	if (applied_effects != NULL) {
		len_appl_eff = strlen(applied_effects) ;
	}

	if (applied_effects == NULL) {
		applied_effects = (char *) malloc(sizeof(char)) ;
		applied_effects[0] = '\0' ;
	}
	applied_effects = (char *) realloc(applied_effects, len_appl_eff+strlen(buffer)+1) ;
	strcat(applied_effects, buffer) ;
}

//Reads kernel properties from stdin and
//applies an erosion with the desired kernel
//to the image
//It also updates the string that holds all effects applied
void apply_erosion() {
	int type, size ;
	read_kernel(&type, &size) ;

	Mat kernel = getStructuringElement(type, Size(2*size+1, 2*size+1), Point(size, size)) ;
	erode(modified, modified, kernel) ;

	write_effects_str(EROSION, type, size) ;	
}

//The same as apply_erosion, but applies a dilation
void apply_dilation() {
	int type, size ;
	read_kernel(&type, &size) ;

	Mat kernel = getStructuringElement(type, Size(2*size+1, 2*size+1), Point(size, size)) ;
	dilate(modified, modified, kernel) ;

	write_effects_str(DILATION, type, size) ;	
}


//The same as apply_erosion
void apply_opening() {
	int type, size ;
	read_kernel(&type, &size) ;

	Mat kernel = getStructuringElement(type, Size(2*size+1, 2*size+1), Point(size, size)) ;
	morphologyEx(modified, modified, MORPH_OPEN, kernel) ;

	write_effects_str(OPENING, type, size) ;	
}

//The same as apply_erosion
void apply_closing() {
	int type, size ;
	read_kernel(&type, &size) ;

	Mat kernel = getStructuringElement(type, Size(2*size+1, 2*size+1), Point(size, size)) ;
	morphologyEx(modified, modified, MORPH_CLOSE, kernel) ;

	write_effects_str(CLOSING, type, size) ;	
}

//changes the trackbars values to the otsu threshold value
void apply_otsu() {
	int lower_value = (int) threshold(gray, binary, 0, 255, THRESH_BINARY_INV | THRESH_OTSU) ;	
	setTrackbarPos(TB_LOWER_NAME, WINDOW_TITLE, lower_value) ;
	setTrackbarPos(TB_UPPER_NAME, WINDOW_TITLE, 255) ;
}


//Show all possible options
//read an option from stdin and calls the 
//desired function
//It returns zero when exit is selected
int effects_menu() {
	printf("Effect menu\n\n"
			"Choose an option:\n"
			"0 - Exit\n"
			"1 - Clear effects\n"
			"2 - Show current effects\n"
			"3 - Apply erosion\n"
			"4 - Apply dilation\n"
			"5 - Apply closing\n"
			"6 - Apply opening\n"
			"7 - Change threshold values\n"
			"8 - Apply Otsu's Binarization\n\n"
			) ;

	int option = 0 ;
	scanf("%d", &option) ;

	switch (option) {
		case 1:
			clear_effects() ;
			break ;
		case 2:
			if (applied_effects == NULL) {
				printf("No effects applied!") ;
			} else {
				printf(applied_effects) ;
			}
			printf("\n") ;
			break ;
		case 3:
			apply_erosion() ;
			break ;
		case 4:
			apply_dilation() ;
			break ;
		case 5:
			apply_opening() ;
			break ;
		case 6:
			apply_closing() ;
			break ;
		case 7:
			printf("Use the trackbas to change the threshold values, then press any key on window.\n") ;
			waitKey(0) ;
			break ;
		case 8:
			apply_otsu() ;
			break ;
	}

	trackbar_event(0,0) ;

	return option ;
}
