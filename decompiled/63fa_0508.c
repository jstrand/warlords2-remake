
void __cdecl16far auto_ui_pillage_sack_raze(int param_1)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_SS;
  char *pcVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined1 local_98 [2];
  undefined1 local_96 [60];
  char local_5a [80];
  undefined1 local_a [8];
  
  FUN_54f6_0000(0xc);
  FUN_78a8_06ae(1,0xf,0,3);
  pcVar3 = get_string(0x44,param_1);
  FUN_7ecb_00d6(0x140,0x40,pcVar3);
  FUN_78a8_06ae(2,0xf,0,3);
  iVar1 = DAT_4125_0a3c * 0x41 + 0x1581;
  uVar5 = 0x3c04;
  pcVar3 = get_string(0x45,param_1);
  sprintf((char *)CONCAT22(unaff_SS,local_5a),pcVar3,iVar1,uVar5);
  FUN_7ecb_00d6(0x140,0x73,local_5a);
  uVar5 = DAT_451b_01b4;
  pcVar3 = get_string(0x46,0);
  sprintf((char *)CONCAT22(unaff_SS,local_5a),pcVar3,uVar5);
  FUN_7ecb_00d6(0x140,0x87,local_5a);
  iVar1 = DAT_451b_01b8;
  pcVar3 = get_string(0x47,(uint)(DAT_451b_01b8 != 1));
  sprintf((char *)CONCAT22(unaff_SS,local_5a),pcVar3,iVar1);
  FUN_7ecb_00d6(0x140,0xa5,local_5a);
  iVar1 = DAT_451b_01b6;
  pcVar3 = get_string(0x48,(uint)(DAT_451b_01b6 != 1));
  uVar2 = (undefined2)((ulong)pcVar3 >> 0x10);
  sprintf((char *)CONCAT22(unaff_SS,local_5a),pcVar3,iVar1);
  FUN_7ecb_00d6(0x140,0xb9,local_5a);
  uVar4 = 4;
  uVar6 = 2;
  uVar5 = FUN_216d_002e(local_a);
  FUN_24d0_02e5(uVar5,uVar2,uVar4,uVar6);
  uVar4 = 2;
  uVar6 = 4;
  uVar5 = FUN_216d_002e(local_a);
  FUN_24d0_02e5(uVar5,uVar2,uVar4,uVar6);
  pcVar3 = get_string(0x44,2);
  FUN_7ecb_00bf(0xc0,0xe6,pcVar3);
  pcVar3 = get_string(0x44,3);
  FUN_7ecb_00bf(0x170,0xe6,pcVar3);
  for (iVar1 = 0; iVar1 < DAT_451b_01b8; iVar1 = iVar1 + 1) {
    FUN_8611_08be(((undefined2 *)&DAT_451b_01ba)[iVar1],current_player,0x342a,0x4125,0xd0,
                  iVar1 * 0x1e + 0xfa,0,0,0,0,1,0,0);
    armytype_lookup(((undefined2 *)&DAT_451b_01ba)[iVar1],local_98);
    FUN_7ecb_00bf(0xf8,iVar1 * 0x1e + 0x100,local_96);
    sprintf((char *)CONCAT22(unaff_SS,local_5a),(char *)0x41250a66,
            ((undefined2 *)&DAT_451b_01ae)[iVar1]);
    FUN_7ecb_00bf(0x170,iVar1 * 0x1e + 0x100,local_5a);
  }
  for (; iVar1 < 3; iVar1 = iVar1 + 1) {
    FUN_8611_08be(0xffff,current_player,0x342a,0x4125,0xd0,iVar1 * 0x1e + 0xfa,0,0,0,0,1,0,0);
  }
  FUN_8065_10fb(0,0);
  FUN_54f6_069c();
  FUN_54f6_069c();
  return;
}


