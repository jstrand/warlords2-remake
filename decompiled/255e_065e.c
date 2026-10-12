
void __cdecl16far sample_play(int param_1,undefined2 param_2)

{
  int iVar1;
  
  if (DAT_4125_37a2 != '\0') {
    if (DAT_4125_37ab != '\0') {
      printf((char *)0x4125387f,7);
      return;
    }
    if (((undefined1 *)&DAT_451b_3108)[param_1 * 0xb] != '\0') {
      if (DAT_4125_37a0 != -1) {
        sample_wait();
      }
      DAT_451b_3106 = 11000;
      DAT_451b_3104 = 0x4125;
      DAT_451b_3102 = 0x704e;
      iVar1 = param_1 * 0xb;
      DAT_451b_3100 = *(undefined2 *)((int)(undefined2 *)&DAT_451b_3109 + iVar1);
      DAT_451b_30fe = *(undefined2 *)((int)(undefined2 *)&DAT_451b_310d + iVar1);
      DAT_451b_30fc = *(undefined2 *)((int)(undefined2 *)&DAT_451b_310b + iVar1);
      DAT_4125_37a0 = param_1;
      DAT_4125_37a7 = param_2;
      digpak_play2();
    }
  }
  return;
}


