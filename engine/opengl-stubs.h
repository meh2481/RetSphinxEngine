#ifndef GL_FUNC
#error GL_FUNC undefined
#endif

#ifndef GL_PTR
#error GL_PTR undefined
#endif

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
GL_FUNC(void,glLoadMatrixf,(const GLfloat *m),(m),)

// drawing
GL_FUNC(void,glVertexPointer,(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer),(size,type,stride,pointer),)
GL_FUNC(void,glTexCoordPointer,(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer),(size,type,stride,pointer),)
GL_FUNC(void,glDrawArrays,(GLenum mode, GLint first, GLsizei count),(mode,first,count),)
GL_FUNC(void,glDrawElements,(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices),(mode,count,type,indices),)

GL_FUNC(void,glVertex3f,(GLfloat x, GLfloat y, GLfloat z),(x,y,z),)
GL_FUNC(void,glVertex3i,(GLint x, GLint y, GLint z),(x,y,z),)
GL_FUNC(void,glVertex2f,(GLfloat x, GLfloat y),(x,y),)
GL_FUNC(void,glPointSize,(GLfloat size),(size),)
GL_FUNC(void,glNormal3f,(GLfloat x, GLfloat y, GLfloat z),(x,y,z),)
GL_FUNC(void,glDeleteLists,(GLuint list, GLsizei range),(list,range),)
GL_FUNC(GLuint,glGenLists,(GLsizei range),(range),return)
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
GL_FUNC(void,glGetLightfv,(GLenum light, GLenum pname, GLfloat *params),(light,pname,params),)
GL_FUNC(void,glLightModelfv,(GLenum pname, const GLfloat *params),(pname, params),)
GL_FUNC(void,glMaterialfv,(GLenum face, GLenum pname, const GLfloat *params),(face, pname, params),)
GL_FUNC(void,glMaterialf,(GLenum face, GLenum pname, const GLfloat param),(face, pname, param),)

GL_FUNC(void,glColorPointer,(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr),(size,type,stride,ptr),)


//Win32 context stuff
#ifdef _WIN32
GL_FUNC(HGLRC,wglCreateContext,(HDC hdc),(hdc),return)
GL_FUNC(HGLRC,wglGetCurrentContext,(void),(),return)
GL_FUNC(BOOL,wglShareLists,(HGLRC hglrc1, HGLRC hglrc2),(hglrc1, hglrc2),return)
GL_FUNC(BOOL,wglDeleteContext,(HGLRC hglrc),(hglrc),return)
#endif



GL_PTR(PFNGLUNIFORM1IPROC, glUniform1i)
GL_PTR(PFNGLUNIFORM1UIPROC, glUniform1ui)
GL_PTR(PFNGLUNIFORM3IPROC, glUniform3i)
GL_PTR(PFNGLUNIFORM3UIPROC, glUniform3ui)
GL_PTR(PFNGLUNIFORM1FPROC, glUniform1f)
GL_PTR(PFNGLUNIFORM3FPROC, glUniform3f)
GL_PTR(PFNGLUNIFORM4FPROC, glUniform4f)
GL_PTR(PFNGLUNIFORM4FPROC, glUniform4fv)
GL_PTR(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv)
GL_PTR(PFNGLUSEPROGRAMPROC,glUseProgram)
GL_PTR(PFNGLBINDVERTEXARRAYPROC,glBindVertexArray)
GL_PTR(PFNGLBINDBUFFERPROC,glBindBuffer)
GL_PTR(PFNGLBUFFERDATAPROC,glBufferData)
GL_PTR(PFNGLDELETEVERTEXARRAYSPROC,glDeleteVertexArrays)
GL_PTR(PFNGLDELETEBUFFERSPROC,glDeleteBuffers)
GL_PTR(PFNGLDETACHSHADERPROC,glDetachShader)
GL_PTR(PFNGLDELETESHADERPROC,glDeleteShader)
GL_PTR(PFNGLDELETEPROGRAMPROC,glDeleteProgram)
GL_PTR(PFNGLCREATEPROGRAMPROC,glCreateProgram)
GL_PTR(PFNGLCREATESHADERPROC,glCreateShader)
GL_PTR(PFNGLSHADERSOURCEPROC,glShaderSource)
GL_PTR(PFNGLCOMPILESHADERPROC,glCompileShader)
GL_PTR(PFNGLATTACHSHADERPROC,glAttachShader)
GL_PTR(PFNGLLINKPROGRAMPROC,glLinkProgram)
GL_PTR(PFNGLGETUNIFORMLOCATIONPROC,glGetUniformLocation)
GL_PTR(PFNGLGETATTRIBLOCATIONPROC,glGetAttribLocation)
GL_PTR(PFNGLGENBUFFERSPROC,glGenBuffers)
GL_PTR(PFNGLGENVERTEXARRAYSPROC,glGenVertexArrays)
GL_PTR(PFNGLENABLEVERTEXATTRIBARRAYPROC,glEnableVertexAttribArray)
GL_PTR(PFNGLVERTEXATTRIBPOINTERPROC,glVertexAttribPointer)
GL_PTR(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap)
GL_PTR(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray)
GL_PTR(PFNGLGETSHADERIVPROC, glGetShaderiv)
GL_PTR(PFNGLISSHADERPROC, glIsShader)
GL_PTR(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog)
GL_PTR(PFNGLISPROGRAMPROC, glIsProgram)
GL_PTR(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog)
GL_PTR(PFNGLGETPROGRAMIVPROC, glGetProgramiv)
GL_PTR(PFNGLVALIDATEPROGRAMPROC, glValidateProgram)
GL_PTR(PFNGLGETACTIVEUNIFORMPROC, glGetActiveUniform)
GL_PTR(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers)
GL_PTR(PFNGLGENRENDERBUFFERSPROC, glGenRenderbuffers)
GL_PTR(PFNGLBINDRENDERBUFFERPROC, glBindRenderbuffer)
GL_PTR(PFNGLRENDERBUFFERSTORAGEPROC, glRenderbufferStorage)
GL_PTR(PFNGLFRAMEBUFFERRENDERBUFFERPROC, glFramebufferRenderbuffer)
GL_PTR(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer)
GL_PTR(PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers)
GL_PTR(PFNGLDELETERENDERBUFFERSPROC, glDeleteRenderbuffers)
GL_PTR(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D)
GL_PTR(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus)





#undef GL_PTR
#undef GL_FUNC

