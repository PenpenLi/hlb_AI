#include "toLuaList_auto.hpp"
#include "GameEntry.h"
#include "scripting/lua-bindings/manual/tolua_fix.h"
#include "scripting/lua-bindings/manual/LuaBasicConversions.h"

int lua_toLuaList_GameEntry_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"GameEntry",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_toLuaList_GameEntry_create'", nullptr);
            return 0;
        }
        cocos2d::Node* ret = GameEntry::create();
        object_to_luaval<cocos2d::Node>(tolua_S, "cc.Node",(cocos2d::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "GameEntry:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_toLuaList_GameEntry_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_toLuaList_GameEntry_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (GameEntry)");
    return 0;
}

int lua_register_toLuaList_GameEntry(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"GameEntry");
    tolua_cclass(tolua_S,"GameEntry","GameEntry","cc.Node",nullptr);

    tolua_beginmodule(tolua_S,"GameEntry");
        tolua_function(tolua_S,"create", lua_toLuaList_GameEntry_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(GameEntry).name();
    g_luaType[typeName] = "GameEntry";
    g_typeCast["GameEntry"] = "GameEntry";
    return 1;
}
TOLUA_API int register_all_toLuaList(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,nullptr,0);
	tolua_beginmodule(tolua_S,nullptr);

	lua_register_toLuaList_GameEntry(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

