
void __cdecl16far auto_ui_version(void)

{
  ulong uVar1;
  undefined2 unaff_SS;
  char *pcVar2;
  char *pcVar3;
  char *pcVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  char *pcVar7;
  char *pcVar8;
  char *pcVar9;
  char local_fe [80];
  char local_ae [80];
  char local_5e [80];
  ulong local_e;
  ulong local_a;
  ulong local_6;
  
  FUN_7ecb_0a1b();
  FUN_1d28_0805(&local_6);
  uVar1 = local_6 / 1000;
  pcVar2 = get_string(0x8b,4);
  sprintf((char *)CONCAT22(unaff_SS,local_5e),pcVar2,uVar1);
  uVar1 = local_a / 1000;
  pcVar2 = get_string(0x8b,5);
  sprintf((char *)CONCAT22(unaff_SS,local_ae),pcVar2,uVar1);
  uVar1 = local_e / 1000;
  pcVar2 = get_string(0x8b,6);
  sprintf((char *)CONCAT22(unaff_SS,local_fe),pcVar2,uVar1);
  FUN_54f6_0000();
  pcVar9 = (char *)CONCAT22(unaff_SS,local_fe);
  pcVar8 = (char *)CONCAT22(unaff_SS,local_ae);
  pcVar7 = (char *)CONCAT22(unaff_SS,local_5e);
  pcVar2 = get_string(0x8b,3);
  uVar6 = 0x4125;
  uVar5 = 0x1292;
  pcVar3 = get_string(0x8b,1);
  pcVar4 = get_string(0x8b,0);
  FUN_8065_1471((char *)pcVar4,(int)((ulong)pcVar4 >> 0x10),pcVar3,uVar5,uVar6,pcVar2,pcVar7,pcVar8,
                pcVar9);
  FUN_8065_10fb(0);
  FUN_54f6_069c();
  return;
}


