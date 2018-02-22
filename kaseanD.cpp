//Kasean Dunham
//Defenders of Space
//2/22/18

class Image {
    public:
	//a texture ID
	GLuint texId;
	int width, height;
	unsigned char *data;
	~Image() {delete [] data; }
	Image() {
	    system("convert asteroid.jpeg asteroid.ppm");
	    FILE *fpi = fopen("asteroid.ppm", "r");
	    if (fpi) {
		char line[200];
		fgets(line, 200, fpi);
		fgets(line, 200, fpi);
		while(line[0] == '#')
		    fgets(line, 200, fpi);
		sscanf(line, "%i %i", &width, &height);
		fgets(line, 200, fpi);
		//get pixel data
		int n = width * height * 3;
		data = new unsigned char[n];
		for(int i=0; i<n; i++)
		    data[i] = fgetc(fpi);
		fclose(fpi);

	    }
	    unlink("asteroid.ppm");
	}} img;

