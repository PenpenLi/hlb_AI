
cc.FileUtils:getInstance():setPopupNotify(false)
cc.FileUtils:getInstance():addSearchPath("src/")
cc.FileUtils:getInstance():addSearchPath("res/")

require "config"
require "cocos.init"
require "game.init"

math.randomseed(os.time())

local function main()
  g_viewManager.setScene(g_Consts.SceneType.game) 
end

local status, msg = xpcall(main, __G__TRACKBACK__)
if not status then
  print(msg)
end
