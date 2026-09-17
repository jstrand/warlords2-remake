// Prepare a WAR2FLAT.EXE import: DS = DGROUP everywhere, then apply the
// labels in war2_labels.txt. Run as a pre-script (before auto-analysis), or
// re-run any time from the Script Manager after adding labels.
// @category War2
import java.io.File;
import java.math.BigInteger;
import java.nio.file.Files;

import ghidra.app.cmd.function.ApplyFunctionSignatureCmd;
import ghidra.app.script.GhidraScript;
import ghidra.app.util.parser.FunctionSignatureParser;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.FunctionDefinitionDataType;
import ghidra.program.model.data.ParameterDefinition;
import ghidra.program.model.data.Pointer;
import ghidra.program.model.data.PointerDataType;
import ghidra.program.model.address.Address;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Function.FunctionUpdateType;
import ghidra.program.model.listing.ParameterImpl;
import ghidra.program.model.listing.ReturnParameterImpl;
import ghidra.program.model.listing.VariableStorage;
import java.util.ArrayList;
import java.util.List;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.SourceType;

public class SetupWar2 extends GhidraScript {
    static final int LOAD_SEG = 0x1000;   // Ghidra's MZ loader base
    static final int DGROUP = 0x3125;     // see docs/formats/exe.md

    @Override
    public void run() throws Exception {
        setDataSegment();
        File dir = getSourceFile().getParentFile().getFile(false);
        int n = 0;
        // Generated names first, hand-made names second so they win.
        n += applyLabels(new File(dir, "war2_auto_labels.txt"), true);
        n += applyLabels(new File(dir, "war2_labels.txt"), false);
        println("applied " + n + " labels");
    }

    // Returns the number of labels applied. Generated (auto) labels only
    // rename functions that still carry a default FUN_ name or an auto_ name.
    private int applyLabels(File labels, boolean generated) throws Exception {
        if (!labels.exists())
            return 0;
        int n = 0;
        for (String line : Files.readAllLines(labels.toPath())) {
            line = line.replaceAll("#.*", "").trim();
            String proto = null;
            int bar = line.indexOf('|');
            if (bar >= 0) {
                proto = line.substring(bar + 1).trim();
                line = line.substring(0, bar).trim();
            }
            if (line.isEmpty())
                continue;
            String[] f = line.split("\\s+");
            String[] so = f[0].split(":");
            int seg = Integer.parseInt(so[0], 16) + LOAD_SEG;
            Address a = toAddr(String.format("%04x:%s", seg, so[1]));
            if (f[1].equals("func")) {
                disassemble(a);
                Function fn = getFunctionAt(a);
                if (generated && fn != null && !fn.getName().startsWith("FUN_")
                        && !fn.getName().startsWith("auto_"))
                    continue;
                if (fn == null)
                    fn = createFunction(a, f[2]);
                if (fn != null) {
                    fn.setName(f[2], SourceType.USER_DEFINED);
                    if (proto != null) {
                        try {
                            applyPrototype(fn, proto);
                        }
                        catch (Exception e) {
                            printerr("bad prototype for " + f[2] + ": " + e.getMessage());
                        }
                    }
                }
                else
                    printerr("could not create function at " + a);
            }
            else {
                createLabel(a, f[2], true, SourceType.USER_DEFINED);
            }
            n++;
        }
        return n;
    }

    private void applyPrototype(Function fn, String proto) throws Exception {
        FunctionSignatureParser p = new FunctionSignatureParser(
                currentProgram.getDataTypeManager(), null);
        FunctionDefinitionDataType sig = p.parse(fn.getSignature(), proto);
        // Large model: data pointers are 4-byte far pointers, but the
        // program's default pointer size is 2.
        sig.setReturnType(far(sig.getReturnType()));
        ParameterDefinition[] args = sig.getArguments();
        for (ParameterDefinition a : args)
            a.setDataType(far(a.getDataType()));
        sig.setArguments(args);
        sig.setCallingConvention("__cdecl16far");
        if (sig.getReturnType().getLength() != 4) {
            if (!new ApplyFunctionSignatureCmd(fn.getEntryPoint(), sig,
                    SourceType.USER_DEFINED).applyTo(currentProgram))
                printerr("could not apply prototype to " + fn.getName());
            return;
        }
        // Borland returns long and far pointers in DX:AX, which Ghidra's
        // 16-bit conventions lack (it would invent a hidden return pointer),
        // so lay the storage out by hand: far call, arguments from [SP+4].
        List<ParameterImpl> params = new ArrayList<>();
        int offset = 4;
        for (ParameterDefinition a : args) {
            params.add(new ParameterImpl(a.getName(), a.getDataType(), offset, currentProgram));
            offset += (a.getDataType().getLength() + 1) & ~1;
        }
        VariableStorage dxax = new VariableStorage(currentProgram,
                currentProgram.getRegister("DX"), currentProgram.getRegister("AX"));
        fn.updateFunction("__cdecl16far",
                new ReturnParameterImpl(sig.getReturnType(), dxax, currentProgram),
                params, FunctionUpdateType.CUSTOM_STORAGE, true, SourceType.USER_DEFINED);
        fn.setVarArgs(sig.hasVarArgs());
    }

    private DataType far(DataType dt) {
        if (dt instanceof Pointer ptr)
            return new PointerDataType(ptr.getDataType(), 4, currentProgram.getDataTypeManager());
        return dt;
    }

    // Borland large model: every far function keeps DS = DGROUP. Telling
    // Ghidra lets [xxxx] operands resolve to DGROUP data.
    private void setDataSegment() throws Exception {
        Register ds = currentProgram.getRegister("ds");
        BigInteger value = BigInteger.valueOf(DGROUP + LOAD_SEG);
        for (MemoryBlock b : currentProgram.getMemory().getBlocks()) {
            if (b.isInitialized() && b.getStart().getAddressSpace().equals(
                    currentProgram.getAddressFactory().getDefaultAddressSpace()))
                currentProgram.getProgramContext().setValue(ds, b.getStart(), b.getEnd(), value);
        }
    }
}
