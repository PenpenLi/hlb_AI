
local viewManager = {}
setmetatable(viewManager,{__index = _G})
setfenv(1,viewManager)

local mLayerRoot 
local layerEnum = {
                    Map    = 1, --地图层
                    UI     = 2, --UI层
                    MsgBox = 3, --pop弹框 
                    Effect = 4, --特效层
                    Guild  = 5, --新手引导
                  }
local layerObjs = {}

--进入新场景时默认显示的layer
local function initSceneLayer(sceneType) 
  if sceneType == g_Consts.SceneType.login then 
  elseif sceneType == g_Consts.SceneType.loading then 
  elseif sceneType == g_Consts.SceneType.game then 
    addNodeForUI(require("game.views.home.HomeView").new()) 
  end 
end 

--切换场景
function setScene(sceneType)
  print("setScene", sceneType)
  if mLayerRoot then 
    mLayerRoot:removeFromParent()
  end

  local newScene = cc.Scene:create()

  mLayerRoot = cc.Layer:create()
  mLayerRoot:setIgnoreAnchorPointForPosition(false)
  mLayerRoot:setAnchorPoint(cc.p(0.5,0.5))
  mLayerRoot:setPosition(display.center)  
  newScene:addChild(mLayerRoot)

  local node 
  for k, zorder in pairs(layerEnum) do 
    node = cc.Node:create()
    node:setIgnoreAnchorPointForPosition(false)
    node:setAnchorPoint(cc.p(0.5,0.5)) 
    node:setPosition(display.center)
    node:setContentSize(display.size)  
    mLayerRoot:addChild(node, zorder)
    layerObjs[zorder] = node 
  end 

  initSceneLayer(sceneType)

  local director = cc.Director:getInstance()
  if director:getRunningScene() then 
    director:replaceScene(newScene)
  else
    director:runWithScene(newScene)
  end  
end 

function addNodeForMap(node)
  if node then
    layerObjs[layerEnum.Map]:addChild(node)
  end
end

function addNodeForUI(node) 
  if node then 
    layerObjs[layerEnum.UI]:addChild(node)
  end
end

function addNodeForMsgBox(node) 
  if node then
    layerObjs[layerEnum.MsgBox]:addChild(node)
  end
end 

function addNodeForEffect(node) 
  if node then
    layerObjs[layerEnum.Effect]:addChild(node)
  end
end 

function addNodeForGuild(node) 
  if node then
    layerObjs[layerEnum.Guild]:addChild(node)
  end
end



return viewManager