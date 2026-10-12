
void __cdecl16far auto_str_it_contains_heroes(void)

{
  bool bVar1;
  int iVar2;
  undefined2 unaff_SS;
  char local_34 [50];
  
  bVar1 = false;
  if (DAT_451b_1f02 != 0) {
    if (DAT_4125_2bda == 0) {
      if (*(char *)((int)DAT_451b_1f02 + 4) == '\x1c') {
        bVar1 = true;
      }
    }
    else {
      for (iVar2 = 0; iVar2 < DAT_451b_1e9e; iVar2 = iVar2 + 1) {
        if (*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar2 * 2) + 4) == '\x1c')
        {
          bVar1 = true;
        }
      }
    }
    strcpy((char *)CONCAT22(unaff_SS,local_34),(char *)0x41252be4);
    if (bVar1) {
      strcpy((char *)CONCAT22(unaff_SS,local_34),s_It_contains_heroes_4125_2be5);
    }
    FUN_7b4c_0088((char *)s_Disband_4125_2bf8,0x4125,(char *)s_Are_you_sure_you_4125_2c00,0x4125,
                  (char *)s_want_to_disband_this_4125_2c11,0x4125,(undefined2 *)&DAT_4125_2c26,
                  0x4125,local_34);
  }
  return;
}


