#ifndef LUA_INTERFACE_H
#define LUA_INTERFACE_H

struct lua_State;

class LuaInterface
{
public:

    LuaInterface(const char *script, int argc, const char * const *argv);
    ~LuaInterface();
    
    bool Init();
    void Shutdown();
    void GC();
    unsigned int MemUsed();

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

protected:

    void lookupFunc(const char *f);
    bool doCall(int nparams, int nrets = 0);

	const char *script;
	int argc;
	const char *const *argv;

    lua_State *_lua;
};

#endif

