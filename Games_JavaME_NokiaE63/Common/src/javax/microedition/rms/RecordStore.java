package javax.microedition.rms;

import java.io.*;
import java.util.Vector;

public class RecordStore {
    private static final String STORE_DIR = ".rms_data";
    private final String name;
    private final Vector records = new Vector();

    private RecordStore(String name) {
        this.name = name;
        load();
    }

    public static RecordStore openRecordStore(String recordStoreName, boolean createIfNecessary) throws RecordStoreException {
        return new RecordStore(recordStoreName);
    }

    public synchronized int addRecord(byte[] data, int offset, int numBytes) throws RecordStoreException {
        byte[] copy = new byte[numBytes];
        System.arraycopy(data, offset, copy, 0, numBytes);
        records.addElement(copy);
        save();
        return records.size();
    }

    public synchronized byte[] getRecord(int recordId) throws RecordStoreException {
        int index = recordId - 1;
        if (index < 0 || index >= records.size()) {
            throw new RecordStoreException("Invalid record ID: " + recordId);
        }
        byte[] original = (byte[]) records.elementAt(index);
        byte[] copy = new byte[original.length];
        System.arraycopy(original, 0, copy, 0, original.length);
        return copy;
    }

    public synchronized void setRecord(int recordId, byte[] data, int offset, int numBytes) throws RecordStoreException {
        int index = recordId - 1;
        if (index < 0 || index >= records.size()) {
            throw new RecordStoreException("Invalid record ID: " + recordId);
        }
        byte[] copy = new byte[numBytes];
        System.arraycopy(data, offset, copy, 0, numBytes);
        records.setElementAt(copy, index);
        save();
    }

    public synchronized int getNumRecords() {
        return records.size();
    }

    public void closeRecordStore() {}

    private void load() {
        File f = new File(STORE_DIR, name + ".rms");
        if (!f.exists()) return;
        try (DataInputStream in = new DataInputStream(new FileInputStream(f))) {
            int count = in.readInt();
            for (int i = 0; i < count; i++) {
                int len = in.readInt();
                byte[] b = new byte[len];
                in.readFully(b);
                records.addElement(b);
            }
        } catch (IOException ignored) {}
    }

    private void save() {
        File dir = new File(STORE_DIR);
        if (!dir.exists()) dir.mkdirs();
        File f = new File(dir, name + ".rms");
        try (DataOutputStream out = new DataOutputStream(new FileOutputStream(f))) {
            out.writeInt(records.size());
            for (int i = 0; i < records.size(); i++) {
                byte[] b = (byte[]) records.elementAt(i);
                out.writeInt(b.length);
                out.write(b);
            }
        } catch (IOException ignored) {}
    }
}
