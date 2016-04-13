#ifndef LUA_INTERFACE_H
#define LUA_INTERFACE_H

struct lua_State;

class LuaObjGlue;

class LuaInterface
{
public:

    LuaInterface(const char *script, int argc, const char * const *argv);
    ~LuaInterface();
    
    bool Init();
    void Shutdown();
    void GC();
    unsigned int MemUsed();
	
	LuaObjGlue *createObject(void *o, unsigned ty, const char *classname);
	void deleteObject(LuaObjGlue *glue);

	//Direct method calls
    bool call(const char *f);
    bool call(const char *f, const char *);
    bool call(const char *f, const char *a, const char *b);
    bool call(const char *f, float);
    bool call(const char *f, int, int);
	bool call(const char *func, int a, int b, bool c);
	bool call(const char *func, int a, int b, int c, int d);
	bool call(const char *func, int a, int b, int c, int d, int e);
	bool call(const char *func, const char *a, const char *b, const char *c, const char *d, const char *e);
	bool call(const char *func, const char *a, const char *b, const char *c, const char *d);
	
	//Method calls on classes
	bool callMethod(void *o, const char *func);
	bool callMethod(void *o, const char *func, float a);
	bool callMethod(void *o, const char *func, float a, float b);
	
	//TODO: Add functions that call luaL_newmetatable() and all that, rather than just getState()
	lua_State* getState() {return _lua;};

protected:

    void lookupFunc(const char *f);
	//void lookupObject(void *o); // push Lua representation of an object
	void lookupMethod(void *o, const char *func); // Push object and its method as function
    bool doCall(int nparams, int nrets = 0);

	const char *script;
	int argc;
	const char *const *argv;

    lua_State *_lua;
};

class LuaObjGlue
{
	friend class LuaInterface;
public:
	LuaObjGlue(void *p, unsigned ty) : obj(p), type(ty) {}
	void *obj; // read-only
	const unsigned type;
	
private:
	LuaObjGlue(const LuaObjGlue&); // non-copyable
};

#endif

