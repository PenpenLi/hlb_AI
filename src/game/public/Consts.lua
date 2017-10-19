--g_Consts
local constsMode = {}
setmetatable(constsMode,{__index = _G})
setfenv(1,constsMode)


SceneType = {
  none    = 1, 
  login   = 2, 
  loading = 3, 
  game    = 4, 
}




Currency = {
  ["Money"] = 1,   --元宝
}





--长连接消息id
NetMsg = {
  ["LoginReq"] = 10000,       --登录
  ["LoginRsp"] = 10001, 
  ["HeartBeatReq"] = 10002,   --心跳包
  ["HeartBeatRsp"] = 10003,
  ["ServerPushReq"] = 10004,  --后台推送
  ["ServerPushRsp"] = 10005,
  ["ChatSendReq"] = 10008,    --聊天
  ["ChatSendRsp"] = 10009,

}


--用户事件枚举
CustomEvent = {
  NewMail    = 1,
  Chat       = 2,
  Guild_Help = 3,
  Queue      = 4,
  PayResult  = 5,
  PlayerTarget=6,
  Item = 7,
  Attacked = 8,
  CloseTower=9,
  KingPoint = 10,
  MerryGoRound = 11,
  Money = 12,
  GuildInvite = 13,
}



return constsMode