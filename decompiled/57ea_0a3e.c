
int __cdecl16far FUN_57ea_0a3e(int param_1,int param_2,int param_3)

{
  int x2;
  int y2;
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = army_count;
  iVar3 = 1000;
LAB_57ea_0b09:
  do {
    do {
      iVar2 = iVar1 + -1;
      if (iVar1 == 0) {
        return iVar3;
      }
      iVar1 = iVar2;
    } while (*(char *)(iVar2 * 0x16 + -0x777b) == '\x0f');
    x2 = *(int *)(iVar2 * 0x16 + -0x7780);
    y2 = *(int *)(iVar2 * 0x16 + -0x777e);
    if (param_3 != -1) goto LAB_57ea_0ab9;
  } while (*(char *)(iVar2 * 0x16 + -0x777b) == (char)current_player);
  goto LAB_57ea_0ace;
LAB_57ea_0ab9:
  if (*(char *)(iVar2 * 0x16 + -0x777b) == param_3) {
LAB_57ea_0ace:
    if (((opt_hidden_map == 0) || (iVar2 = FUN_623c_16ae(x2,y2), iVar2 != 0)) &&
       (iVar2 = map_distance(param_1,param_2,x2,y2), iVar2 < iVar3)) {
      iVar3 = iVar2;
    }
  }
  goto LAB_57ea_0b09;
}


