
void __cdecl16far auto_ui_army_bonus(int param_1)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 in_DX;
  undefined2 uVar3;
  int iVar4;
  undefined2 unaff_SS;
  char *pcVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  undefined1 local_4c [2];
  undefined1 local_4a [20];
  undefined2 local_36;
  undefined2 local_30;
  undefined1 local_e [4];
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  if (param_1 == 0) {
    FUN_78a8_06ae(1,0xf,0,3);
    pcVar5 = get_string(0xa4,0);
    uVar3 = (undefined2)((ulong)pcVar5 >> 0x10);
    FUN_7ecb_00d6(0x140,0x3e,pcVar5);
    uVar6 = 4;
    uVar7 = 2;
    uVar2 = FUN_216d_002e(&local_a);
    FUN_24d0_02e5(uVar2,uVar3,uVar6,uVar7);
    FUN_24d0_0002(0);
    local_a = local_a + 1;
    local_8 = local_8 + 1;
    local_6 = local_6 + -2;
    local_4 = local_4 + -2;
    FUN_216d_01fd(&local_a);
    FUN_216d_002e(&local_a);
    FUN_216d_000e(local_e);
    __F_SPUSH(local_e._0_2_,local_e._2_2_);
    FUN_1997_027e(0x2e);
  }
  else {
    FUN_54f6_070b(0x30,0x58,0x1a8,0xb6);
    unaff_SS = in_DX;
  }
  FUN_78a8_06ae(2,0xf,0,3);
  for (iVar4 = 0; iVar4 < 6; iVar4 = iVar4 + 1) {
    iVar1 = *(int *)((int)DAT_451b_287a + (DAT_451b_2878 + iVar4) * 2);
    FUN_8611_08be(iVar1,current_player,0x342a,0x4125,0x80,iVar4 * 0x1e + 0x94,0,0,0,0,
                  current_player + 2,0,0);
    armytype_lookup(iVar1,local_4c);
    FUN_7ecb_00bf(0xa8,iVar4 * 0x1e + 0x99,local_4a);
    sprintf(&DAT_451b_1d28,(char *)0x41253249,local_36);
    FUN_7ecb_00bf(0x118,iVar4 * 0x1e + 0x99,(undefined1 *)&DAT_451b_1d28,0x4125);
    sprintf(&DAT_451b_1d28,(char *)0x4125324c,local_30);
    FUN_7ecb_00bf(0x148,iVar4 * 0x1e + 0x99,(undefined1 *)&DAT_451b_1d28,0x4125);
    FUN_216d_000e(local_e);
    if (((((undefined1 *)&army_move_flags)[iVar1 * 6] != '\0') ||
        (((((undefined1 *)&DAT_451b_0665)[iVar1 * 6] != '\0' &&
          (((undefined1 *)&DAT_451b_0666)[iVar1 * 6] != '\0')) ||
         (((undefined1 *)&DAT_451b_0665)[iVar1 * 6] != '\0')))) ||
       (((undefined1 *)&DAT_451b_0666)[iVar1 * 6] != '\0')) {
      unaff_SS = 0x4125;
      __F_SPUSH(local_e._0_2_,local_e._2_2_);
      FUN_1997_0129(8);
    }
    uVar2 = auto_ui_army_bonus_text(iVar1,0,0);
    FUN_7ecb_00bf(400,iVar4 * 0x1e + 0x9a,uVar2,unaff_SS);
  }
  return;
}


