
// GL state
GL_FUNC(void,glGetIntegerv,(GLenum pname, GLint *params),(pname,params),)
GL_FUNC(const GLubyte *,glGetString,(GLenum name),(name),return)
GL_FUNC(void,glOrtho,(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar),(left,right,bottom,top,zNear,zFar),)
GL_FUNC(void,glPopAttrib,(void),(),)
GL_FUNC(void,glPushAttrib,(GLbitfield mask),(mask),)
GL_FUNC(GLenum,glGetError,(void),(),return)
GL_FUNC(void,glGetFloatv,(GLenum pname, GLfloat *params),(pname,params),)
GL_FUNC(void,glGetDoublev,(GLenum pname, GLdouble *params),(pname,params),)
GL_FUNC(void,glGetTexParameterfv,(GLenum target, GLenum pname, GLfloat *params),(target,pname,params),)
GL_FUNC(void,glViewport,(GLint x, GLint y, GLsizei width, GLsizei height),(x,y,width,height),)
GL_FUNC(void,glScissor,(GLint x, GLint y, GLsizei width, GLsizei height),(x,y,width,height),)
GL_FUNC(void,glBlendFunc,(GLenum f,GLenum x),(f,x),)
GL_FUNC(void,glClear,(GLbitfield a),(a),)
GL_FUNC(void,glClearColor,(GLclampf r,GLclampf g,GLclampf b,GLclampf a),(r,g,b,a),)
GL_FUNC(void,glDisable,(GLenum cap),(cap),)
GL_FUNC(void,glDisableClientState,(GLenum array),(array),)
GL_FUNC(void,glEnable,(GLenum cap),(cap),)
GL_FUNC(void,glEnableClientState,(GLenum array),(array),)
GL_FUNC(void,glFinish,(void),(),)
GL_FUNC(void,glFlush,(void),(),)
GL_FUNC(void,glMatrixMode,(GLenum mode),(mode),)
GL_FUNC(void,glReadPixels,(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid * data),(x,y,width,height,format,type,data),)

// textures
GL_FUNC(void,glGenTextures,(GLsizei n, GLuint *textures),(n,textures),)
#ifdef __APPLE__
GL_FUNC(void,glTexImage2D,(GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels),(target,level,internalFormat,width,height,border,format,type,pixels),)
#else
GL_FUNC(void,glTexImage2D,(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels),(target,level,internalFormat,width,height,border,format,type,pixels),)
#endif
GL_FUNC(void,glBindTexture,(GLenum target,GLuint name),(target,name),)
GL_FUNC(void,glDeleteTextures,(GLsizei n, const GLuint *textures),(n,textures),)
GL_FUNC(void,glTexParameterf,(GLenum target, GLenum pname, GLfloat param),(target,pname,param),)
GL_FUNC(void,glTexParameteri,(GLenum target, GLenum pname, GLint param),(target,pname,param),)
GL_FUNC(void,glPixelStorei,(GLenum pname, GLint param),(pname,param),)
GL_FUNC(void,glTexSubImage2D,(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels),(target,level,xoffset,yoffset,width,height, format,type,pixels),)

// deprecated?
GL_FUNC(void,glBegin,(GLenum e),(e),)
GL_FUNC(void,glEnd,(void),(),)

// matrix stack - deprecated
GL_FUNC(void,glLoadIdentity,(void),(),)
GL_FUNC(void,glPopMatrix,(void),(),)
GL_FUNC(void,glPushMatrix,(void),(),)
GL_FUNC(void,glRotatef,(GLfloat angle, GLfloat x, GLfloat y, GLfloat z),(angle,x,y,z),)
GL_FUNC(void,glScalef,(GLfloat x, GLfloat y, GLfloat z),(x,y,z),)
GL_FUNC(void,glTranslatef,(GLfloat x, GLfloat y, GLfloat z),(x,y,z),)

// drawing
GL_FUNC(void,glVertexPointer,(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer),(size,type,stride,pointer),)
GL_FUNC(void,glTexCoordPointer,(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer),(size,type,stride,pointer),)
GL_FUNC(void,glDrawArrays,(GLenum mode, GLint first, GLsizei count),(mode,first,count),)

GL_FUNC(void,glVertex3f,(GLfloat x, GLfloat y, GLfloat z),(x,y,z),)
GL_FUNC(void,glVertex3i,(GLint x, GLint y, GLint z),(x,y,z),)
GL_FUNC(void,glVertex2f,(GLfloat x, GLfloat y),(x,y),)
GL_FUNC(void,glPointSize,(GLfloat size),(size),)
GL_FUNC(void,glNormal3f,(GLfloat x, GLfloat y, GLfloat z),(x,y,z),)
GL_FUNC(void,glDeleteLists,(GLuint list, GLsizei range),(list,range),)
GL_FUNC(GLuint,glGenLists,(GLsizei range),(range),)
GL_FUNC(void,glNewList,(GLuint list, GLenum mode),(list,mode),)
GL_FUNC(void,glTexCoord2f,(GLfloat u, GLfloat v),(u,v),)
GL_FUNC(void,glEndList,(void),(),)
GL_FUNC(void,glPolygonMode,(GLenum face, GLenum mode),(face,mode),)
GL_FUNC(void,glCallList,(GLuint list),(list),)
GL_FUNC(void,glColor4f,(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha),(red,green,blue,alpha),)
GL_FUNC(void,glColor3f,(GLfloat red, GLfloat green, GLfloat blue),(red,green,blue),)
GL_FUNC(void,glClearDepth,(GLdouble depth),(depth),)
GL_FUNC(void,glDepthFunc,(GLenum func),(func),)
GL_FUNC(void,glHint,(GLenum target,  GLenum mode),(target,mode),)
GL_FUNC(void,glShadeModel,(GLenum  mode),(mode),)
GL_FUNC(void,glLightfv,(GLenum light, GLenum pname, const GLfloat *params),(light,pname,params),)

//Win32 context stuff
#ifdef _WIN32
#include <windows.h>
GL_FUNC(HGLRC,wglCreateContext,(HDC hdc),(hdc),)
GL_FUNC(HGLRC,wglGetCurrentContext,(void),(),)
GL_FUNC(BOOL,wglShareLists,(HGLRC hglrc1, HGLRC hglrc2),(hglrc1, hglrc2),)
GL_FUNC(BOOL,wglDeleteContext,(HGLRC hglrc),(hglrc),)
#endif










