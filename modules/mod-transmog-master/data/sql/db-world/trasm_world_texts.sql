SET @TEXT_ID := 601083;
DELETE FROM `npc_text` WHERE `ID` IN  (@TEXT_ID,@TEXT_ID+1);
INSERT INTO `npc_text` (`ID`, `text0_0`) VALUES
(@TEXT_ID, '织幻者哈沙姆允许你在不更改装备属性的情况下更改装备外观。\r\n物品幻化后将与你绑定且不可退还！\r\n刷新菜单菜单选项以示价格。\r\n\r\n并非所有装备都可以幻化。\r\n限制条件包括但不限于：\r\n只有装备和武器可以幻化\r\n枪，弓和弩可以相互幻化\r\n不能幻化钓鱼竿\r\n你必须能够装备该物品，幻化才能被接受。\r\n\r\n只要你拥有它们，幻化就会保留在你的物品上。\r\n如果你试图将幻化后的物品放入公会银行或邮寄给其他人，则幻化将被解除。\r\n\r\n你也可以在我这里免费移除幻化！'),
(@TEXT_ID+1, '你可以保存自己设置的多个幻化。\r\n\r\n如果需要保存，首先你必须装备幻化后的物品。\r\n然后当你进入设置管理菜单并进入保存方案设置菜单时，将显示你已身穿幻化后的物品，以便你看到你正在保存的东西。\r\n如果你认为该幻化是正确的，你可以点击保存该配置为方案并按照你的意愿命名。\r\n\r\n要使用某个方案设定，你可以在设置管理菜单中单击保存的设置方案，然后选择使用它们。\r\n如果这套装备已经幻化了，那么旧的幻化就失效了。\r\n\r\n若要删除一个方案，可以转到菜单并选择“删除方案”。');

SET @STRING_ENTRY := 11100;
DELETE FROM `trinity_string` WHERE `entry` IN  (@STRING_ENTRY+0,@STRING_ENTRY+1,@STRING_ENTRY+2,@STRING_ENTRY+3,@STRING_ENTRY+4,@STRING_ENTRY+5,@STRING_ENTRY+6,@STRING_ENTRY+7,@STRING_ENTRY+8,@STRING_ENTRY+9,@STRING_ENTRY+10);
INSERT INTO `trinity_string` (`entry`, `content_default`) VALUES
(@STRING_ENTRY+0, '幻化成功！'),
(@STRING_ENTRY+1, '设备槽空'),
(@STRING_ENTRY+2, '选定的源项目无效'),
(@STRING_ENTRY+3, '源项不存在'),
(@STRING_ENTRY+4, '目标项不存在'),
(@STRING_ENTRY+5, '选定项目无效'),
(@STRING_ENTRY+6, '没有足够的金币'),
(@STRING_ENTRY+7, '你没有足够的代币'),
(@STRING_ENTRY+8, '幻化已删除'),
(@STRING_ENTRY+9, '没有幻化'),
(@STRING_ENTRY+10, '无效的名称');
