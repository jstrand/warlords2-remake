// Decompile functions by name or address and print the C.
// Args: <name-or-seg:off> ...   (Ghidra addresses, i.e. segment + 0x1000)
// @category War2
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;

public class Decompile extends GhidraScript {
    @Override
    public void run() throws Exception {
        DecompInterface dec = new DecompInterface();
        dec.openProgram(currentProgram);
        for (String arg : getScriptArgs()) {
            Function f = arg.contains(":") ? getFunctionAt(toAddr(arg))
                    : getGlobalFunctions(arg).stream().findFirst().orElse(null);
            if (f == null) {
                printerr("no function " + arg);
                continue;
            }
            var res = dec.decompileFunction(f, 120, monitor).getDecompiledFunction();
            println("// " + f.getEntryPoint() + "\n" + (res == null ? "// failed" : res.getC()));
        }
    }
}
