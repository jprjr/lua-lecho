#include <lua.h>
#include <lauxlib.h>

#if !defined LUA_VERSION_NUM || LUA_VERSION_NUM==501
static void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
  luaL_checkstack(L, nup+1, "too many upvalues");
  for (; l->name != NULL; l++) {  /* fill the table with given functions */
    int i;
    lua_pushstring(L, l->name);
    for (i = 0; i < nup; i++)  /* copy upvalues to the top */
      lua_pushvalue(L, -(nup+1));
    lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
    lua_settable(L, -(nup + 3));
  }
  lua_pop(L, nup);  /* remove upvalues */
}
#endif

#ifdef WIN32
#include <windows.h>
#include <io.h>
typedef DWORD t_mode;
#else
#include <termios.h>
#include <unistd.h>
typedef struct termios t_mode;
#endif


int lecho_off(void *userdata) {
    t_mode *state = (t_mode *)userdata;
#ifdef WIN32
    *state &= ~(ENABLE_ECHO_INPUT);
    if(!SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),*state)) {
        return GetLastError();
    }
    return 0;
#else
    state->c_lflag &= ~(ECHO);
    return tcsetattr(0, TCSANOW, state);
#endif
}

int lecho_on(void *userdata) {
    t_mode *state = (t_mode *)userdata;

#ifdef WIN32
    *state |= ENABLE_ECHO_INPUT;
    if(!SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),*state)) {
        return GetLastError();
    }
    return 0;
#else
    state->c_lflag |= ECHO;
    return tcsetattr(0, TCSANOW, state);
#endif
}

int lecho_status(void *userdata) {
    t_mode *state = (t_mode *)userdata;
#ifdef WIN32
    return *state & ENABLE_ECHO_INPUT;
#else
    return state->c_lflag & ECHO;
#endif
}

int lecho_get(void *userdata) {
    t_mode *state = (t_mode *)userdata;

#ifdef WIN32
    if(!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),state)) {
        return GetLastError();
    }
    return 0;
#else
    return tcgetattr(0,state);
#endif
}

size_t lecho_size(void) {
    return sizeof(t_mode);
}


int lecho_lua_off(lua_State *L) {
    t_mode *state;

    lua_getfield(L,-1,"state");
    state = (t_mode *)lua_touserdata(L,-1);

    if(lecho_off(state) == 0) {
        lua_pushboolean(L,1);
    }
    else {
        lua_pushboolean(L,0);
    }

    return 1;
}

int lecho_lua_on(lua_State *L) {
    t_mode *state;

    lua_getfield(L,-1,"state");
    state = (t_mode *)lua_touserdata(L,-1);

    if(lecho_on(state) == 0) {
        lua_pushboolean(L,1);
    }
    else {
        lua_pushboolean(L,0);
    }

    return 1;
}

int lecho_lua_status(lua_State *L) {
    t_mode *state;
    lua_getfield(L,-1,"state");
    state = (t_mode *)lua_touserdata(L,-1);

    lua_pushboolean(L,lecho_status(state));
    return 1;
}

static const struct luaL_Reg lecho_methods[] = {
    { "off"    , lecho_lua_off     },
    { "on"     , lecho_lua_on      },
    { "status" , lecho_lua_status  },
    { NULL     , NULL              },
};

int luaopen_lecho_core(lua_State *L) {
    t_mode *state;

    lua_newtable(L);

    lua_pushvalue(L,-1);
    lua_setfield(L,-1,"__index");

    lua_pushvalue(L,-1);
    lua_setmetatable(L,-1);

    luaL_setfuncs(L,lecho_methods,0);

    state = (t_mode *)lua_newuserdata(L,lecho_size());
    lua_setfield(L,-2,"state");

    lecho_get(state);

    return 1;
}
