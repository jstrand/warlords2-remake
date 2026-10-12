
void __cdecl16far path_init(void)

{
  undefined2 in_DX;
  
  DAT_451b_0026._0_2_ = mem_alloc(0x8880,0,0);
  DAT_451b_0026._2_2_ = in_DX;
  DAT_451b_0022._0_2_ = mem_alloc(0x4440,0,0);
  DAT_451b_0022._2_2_ = in_DX;
  DAT_451b_001e._0_2_ = mem_alloc(0x10b8,0,0);
  DAT_4125_00de = 1;
  DAT_451b_001e._2_2_ = in_DX;
  path_cache_reset_all();
  return;
}


