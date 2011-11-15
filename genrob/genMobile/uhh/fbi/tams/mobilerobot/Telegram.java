package uhh.fbi.tams.mobilerobot;



/**
 * The purpose of this class is to encapsulate constants and means when dealing
 * with telegrams (reading from a stream or writing to a stream).
 */
class Telegram
{

  /**
   * Each telegram starts with this number.
   * They are contained in the first 4 bytes of a telegram.
   */
  static final int MAGIC = 0x4F4C4548;

  /**
   * The current limitation for telegrams is this.
   * This is the size of all bytes - including magic, size, command and CRC.
   */
  static final int BYTESIZE_MAX = 8 * 1024;

  /**
   * Put an integer value at a specific byte position in the correct byte order into
   * a byte buffer.
   * 4 subsequent positions in the buffer have to exist to let this method succeed.
   *
   * @param   ab              Buffer of sufficient size to keep bytes of value at the offset
   * @param   iByteOffset     Position to put bytes into the buffer
   * @param   iValue          Integer value to put
   * @throw   IndexOutOfBoundsException   If buffer size and offset do not match
   */
  static void putInteger (byte[] ab, int iByteOffset, int iValue)
  {
    ab[iByteOffset++] = (byte) (iValue & 0xFF);
    ab[iByteOffset++] = (byte) ((iValue >>> 8) & 0xFF);
    ab[iByteOffset++] = (byte) ((iValue >>> 16) & 0xFF);
    ab[iByteOffset++] = (byte) (iValue >>> 24);
  }


  /**
   * Get an integer value from a specific byte position in the correct byte order out of
   * a byte buffer.
   * 4 subsequent positions in the buffer have to exist to let this method succeed.
   *
   * @param   ab              Buffer of sufficient size to get bytes of value at the offset
   * @param   iByteOffset     Position to get bytes out of the buffer
   * @return  Integer value from the specified byte position
   * @throw   IndexOutOfBoundsException   If buffer size and offset do not match
   */
  static int getInteger (byte[] ab, int iByteOffset)
  {
    int iResult = 0;

    int i = ab[iByteOffset++];
    int m = i << 0;
    int j = 0xFF << 0;
    int k = m & j;
    int l = iResult | k;
    iResult = l;

    i = ab[iByteOffset++];
    m = i << 8;
    j = 0xFF << 8;
    k = m & j;
    l = iResult | k;
    iResult = l;

    i = ab[iByteOffset++];
    m = i << 16;
    j = 0xFF << 16;
    k = m & j;
    l = iResult | k;
    iResult = l;

    i = ab[iByteOffset++];
    m = i << 24;
    j = 0xFF << 24;
    k = m & j;
    l = iResult | k;
    iResult = l;

    return iResult;
  }


  /**
   * Put a float value at a specific byte position in the correct byte order into
   * a byte buffer.
   * 4 subsequent positions in the buffer have to exist to let this method succeed.
   *
   * @param   ab              Buffer of sufficient size to keep bytes of value at the offset
   * @param   iByteOffset     Position to put bytes into the buffer
   * @param   fValue          Float value to put
   * @throw   IndexOutOfBoundsException   If buffer size and offset do not match
   */
  static void putFloat (byte[] ab, int iByteOffset, float fValue)
  {
    putInteger (ab, iByteOffset, Float.floatToRawIntBits (fValue));
  }


  /**
   * Get a float value from a specific byte position in the correct byte order out of
   * a byte buffer.
   * 4 subsequent positions in the buffer have to exist to let this method succeed.
   *
   * @param   ab              Buffer of sufficient size to get bytes of value at the offset
   * @param   iByteOffset     Position to get bytes out of the buffer
   * @return  Float value from the specified byte position
   * @throw   IndexOutOfBoundsException   If buffer size and offset do not match
   */
  static float getFloat (byte[] ab, int iByteOffset)
  {
    int iValue = getInteger (ab, iByteOffset);

    return Float.intBitsToFloat (iValue);
  }

}
