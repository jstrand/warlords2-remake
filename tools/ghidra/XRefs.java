// List the instructions that reference an address, with the function they're in.
// Args: <seg:off> ...   (Ghidra addresses, i.e. segment + 0x1000)
// Data in DGROUP is usually reached as `[3c04:xxxx]`; pass that form.
// @category War2
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Reference;

public class XRefs extends GhidraScript {
    @Override
    public void run() throws Exception {
        for (String arg : getScriptArgs()) {
            Address target = toAddr(arg);
            println("== " + arg);
            for (Reference r : getReferencesTo(target)) {
                Address from = r.getFromAddress();
                Function f = getFunctionContaining(from);
                println("   " + from + "  " + r.getReferenceType()
                        + (f == null ? "" : "  in " + f.getName() + " @ " + f.getEntryPoint()));
            }
        }
    }
}
