
void __cdecl16far FUN_6a89_1294(void)

{
  undefined1 uVar1;
  int iVar2;
  int unaff_SI;
  int iVar3;
  int unaff_DI;
  int iVar4;
  
  iVar2 = 0;
  while ((iVar3 = unaff_SI, iVar2 < 0x1d &&
         (iVar3 = iVar2, *(char *)(current_player * 0x1d + iVar2 + 0x60b) != DAT_4125_0d56))) {
    iVar2 = iVar2 + 1;
  }
  iVar2 = 0;
  while ((iVar4 = unaff_DI, iVar2 < 0x1d &&
         (iVar4 = iVar2, (int)*(char *)(current_player * 0x1d + iVar2 + 0x60b) != DAT_4125_0d56 + 1)
         )) {
    iVar2 = iVar2 + 1;
  }
  uVar1 = *(undefined1 *)(current_player * 0x1d + iVar3 + 0x60b);
  *(undefined1 *)(current_player * 0x1d + iVar3 + 0x60b) =
       *(undefined1 *)(current_player * 0x1d + iVar4 + 0x60b);
  *(undefined1 *)(current_player * 0x1d + iVar4 + 0x60b) = uVar1;
  DAT_4125_0d56 = DAT_4125_0d56 + 1;
  auto_ui_fighting_order(1);
  FUN_6a89_1067();
  return;
}


