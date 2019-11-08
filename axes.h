#ifndef __drawAXES__
#define __drawAXES__

#include "glut.h"

class Axes //: public renderableParent
{
protected:

	void drawLabelledAxesP(GLfloat len, GLfloat TxtSize);

public:
	GLfloat length;
	GLfloat textSize;

	void setlen(GLfloat len);
	void setTextSize(GLfloat t);

	virtual int render();
	void drawAxesP(GLfloat len);
	Axes();
};

#endif
