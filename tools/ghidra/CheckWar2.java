// Sanity check of an analysed WAR2FLAT.EXE: counts, dice callers, a decompile.
// @category War2
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;

public class CheckWar2 extends GhidraScript {
    @Override
    public void run() throws Exception {
        println("functions: " + currentProgram.getFunctionManager().getFunctionCount());
        Function dice = getGlobalFunctions("dice").get(0);
        var callers = dice.getCallingFunctions(monitor);
        println("dice callers: " + callers.size() + ", call refs: "
                + getReferencesTo(dice.getEntryPoint()).length);
        DecompInterface d = new DecompInterface();
        d.openProgram(currentProgram);
        for (String name : new String[] {"dice", "load_string_files"})
            println(d.decompileFunction(getGlobalFunctions(name).get(0), 60, monitor)
                    .getDecompiledFunction().getC());
    }
}
