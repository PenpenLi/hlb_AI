
local HomeView = class("HomeView",require("game.views.base.BaseLayer"))

function HomeView:ctor()
  HomeView.super.ctor(self)
end 

function HomeView:onEnter()
  -- local layer = cc.CSLoader:createNode("csb/home/home.csb")
  
  -- local layer = g_gameUtil.loadCocosUI("csb/home/home.csb", 5)
  -- if layer then 
  --   self:addChild(layer)
  -- end 

  local node = GameEntry:create()
  self:addChild(node)
end 

function HomeView:onExit() 
end 




return HomeView 
