
int __cdecl16far FUN_828e_06fd(int param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 local_4;
  
  iVar3 = -1;
  local_4 = 1000;
  iVar2 = 0;
  do {
    if (site_count <= iVar2) {
      return iVar3;
    }
    if ((*(uint *)(iVar2 * 0x1f + 0x82e) & 1 << ((byte)current_player & 0x1f)) != 0) {
      if ((opt_hidden_map != 0) && (param_3 != 0)) {
        iVar1 = FUN_8611_15ff(1,*(undefined2 *)(iVar2 * 0x1f + 0x811),
                              *(undefined2 *)(iVar2 * 0x1f + 0x813));
        if (iVar1 == 0) goto LAB_828e_07a9;
      }
      iVar1 = map_distance(param_1,param_2,*(int *)(iVar2 * 0x1f + 0x811),
                           *(int *)(iVar2 * 0x1f + 0x813));
      if (iVar1 < local_4) {
        iVar3 = iVar2;
        local_4 = iVar1;
      }
    }
LAB_828e_07a9:
    iVar2 = iVar2 + 1;
  } while( true );
}


