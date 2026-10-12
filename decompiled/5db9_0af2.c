
void __cdecl16far ai_rebuilding(void)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  
  uVar3 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  iVar2 = (int)DAT_451b_018e;
  if ((*(int *)(iVar2 + 10) * city_count) / 0x50 < 5) {
    iVar1 = 5;
  }
  else {
    iVar1 = (*(int *)(iVar2 + 10) * city_count) / 0x50;
  }
  if (((4 < *(int *)(iVar2 + 0xe)) || (*(int *)(iVar2 + 2) <= iVar1 * *(int *)(iVar2 + 0xe))) &&
     (499 < *(int *)(current_player * 0x14 + 0x185))) {
    if (*(int *)(current_player * 0x14 + 0x185) < 0x7d1) {
      uVar3 = *(undefined2 *)(iVar2 + 0x10);
    }
    else {
      uVar3 = *(undefined2 *)(iVar2 + 0x12);
    }
    iVar2 = ai_city_for_type(uVar3);
    if ((iVar2 != -1) && (499 < *(int *)(current_player * 0x14 + 0x185))) {
      ai_buy_production_type(uVar3,iVar2);
    }
  }
  return;
}


