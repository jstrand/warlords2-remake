// Print memory blocks and entry point (used to find the load segment).
// @category War2
import ghidra.app.script.GhidraScript;
import ghidra.program.model.mem.MemoryBlock;

public class ProbeLayout extends GhidraScript {
    @Override
    public void run() throws Exception {
        println("image base " + currentProgram.getImageBase());
        for (MemoryBlock b : currentProgram.getMemory().getBlocks())
            println("block " + b.getName() + " " + b.getStart() + " - " + b.getEnd());
        for (var a : currentProgram.getSymbolTable().getExternalEntryPointIterator())
            println("entry " + a);
    }
}
