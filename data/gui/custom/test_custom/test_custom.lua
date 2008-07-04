print("init");

function onLogin(name)
	print("user login : "..name);
	listView = getGuiElementByName("serverListView");
	item = loadXml("test_custom_item.xml");
	text = item:getElement(0);
	text:setString(name);
	listView:pushBack(item);
end

function onLogout(name)
	print("user logout : "..name);
	listView = getGuiElementByName("serverListView");
  local elCount = listView:getCount();
  for i=0,elCount do
    item = listView:getElement(i);
    elName = item:getElement(0):getString();
    print(elName);
    if (elName==name) then
    	listView:removeElement(i);
    	return;
    end
  end
end

function update()
	--print("update");
end

function test_func()
	print("test func");
end

function test_kick()
	print("test click");
	listView = getGuiElementByName("serverListView");
	hbox = getGuiElementByName("serverListView"):getSelected();
	text = hbox:getElement(0);
	sendCommand("kick "..text:getString());
end

function test_ban()
	print("test click");
	listView = getGuiElementByName("serverListView");
	hbox = getGuiElementByName("serverListView"):getSelected();
	text = hbox:getElement(0);
	sendToConsole("/ban "..text:getString().." ");
end

