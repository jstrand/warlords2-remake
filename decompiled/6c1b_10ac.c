
void __cdecl16far FUN_6c1b_10ac(void)

{
  int iVar1;
  
  FUN_834b_0000(0x10000,0);
  for (iVar1 = 0; iVar1 < DAT_4125_0d68; iVar1 = iVar1 + 1) {
    if (iVar1 != DAT_4125_0d6a) {
      draw_map_figure(*(int *)(((undefined2 *)&DAT_451b_04ae)[iVar1] * 0x16 + -0x7780),
                      *(int *)(((undefined2 *)&DAT_451b_04ae)[iVar1] * 0x16 + -0x777e),0);
    }
  }
  draw_map_figure(*(int *)(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7780),
                  *(int *)(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x777e),1);
  FUN_834b_04d3(0);
  return;
}


