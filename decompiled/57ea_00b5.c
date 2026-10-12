
int __cdecl16far ai_neutral_targets(int param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int city;
  uint unaff_DI;
  int iVar3;
  undefined2 uVar4;
  uint unaff_SS;
  
  iVar3 = 0;
  iVar2 = city_count;
LAB_57ea_01e6:
  do {
    do {
      city = iVar2 + -1;
      if (iVar2 == 0) {
        return iVar3;
      }
      iVar2 = city;
    } while (*(char *)(city * 0x41 + 0x1592) != current_player);
    uVar4 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  } while ((param_1 != 0) && (*(char *)((int)DAT_451b_018e + city + 0x56) != '\x01'));
  if (*(char *)((int)DAT_451b_018e + city + 0x56) == '\x01') {
    ai_city_garrison_check(city,1);
    *(undefined1 *)((int)DAT_451b_018e + city + 0x56) = 3;
  }
  iVar1 = ai_neutral_neighbours(city,(char *)(ulong)unaff_SS,(char *)((ulong)unaff_DI << 0x10));
  if (iVar1 == 0) goto code_r0x00057fd9;
  goto LAB_57ea_0182;
code_r0x00057fd9:
  uVar4 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  iVar1 = (int)DAT_451b_018e;
  if ((*(char *)(iVar1 + city + 0x56) == '\x03') || (*(char *)(iVar1 + city + 0x56) == '\x02')) {
    *(undefined1 *)(iVar1 + city + 0x56) = 4;
  }
  if (((opt_quick_start == 0) && (*(char *)((int)DAT_451b_018e + city + 0x56) == '\x05')) &&
     (iVar1 = game_turn(), iVar1 < 6)) {
LAB_57ea_0182:
    iVar3 = iVar3 + 1;
    uVar4 = (undefined2)((ulong)param_2 >> 0x10);
    iVar1 = FUN_57ea_03aa(city,(int)param_2,uVar4);
    if (iVar1 == 0) {
      iVar1 = FUN_57ea_0935(city,(int)param_2,uVar4);
      uVar4 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      if (iVar1 == 0xff) {
        *(undefined1 *)((int)DAT_451b_018e + city + 0x56) = 4;
      }
      else {
        *(undefined1 *)((int)DAT_451b_018e + city + 0x56) = 3;
      }
    }
    else {
      iVar1 = game_turn();
      FUN_57ea_0b19(city,10 < iVar1);
      *(undefined1 *)((int)DAT_451b_018e + city + 0x56) = 2;
    }
  }
  goto LAB_57ea_01e6;
}


