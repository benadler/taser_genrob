package uhh.fbi.tams.mobilerobot;

/**
 * This class is used to calculate the 32 bit CRC needed for the binary protocol.
 * There is no need to instantiate this class.
 * The algorithm has been taken from some CC file.
 * @see calculate
 */
class CRC
{

  /**
   * This number of integers will be initialized once in the static initializer of
   * the class.
   * It is used for subsequent calculations of CRC's.
   */
  private static /*unsigned*/ int[] crctab = new int[256];

  /**
   * For subsequent calculations of CRC's a number of integer values will be used
   * and has to be initialized here.
   * @see crctab
   * @see calculate
   */
  static
  {
    for (int i = 0; i < 256; ++i)
    {
      /*unsigned*/
      int crc = i << 24;

      for (int j = 0; j < 8; ++j)
      {
        if ((crc & 0x80000000) != 0)
        {
          crc = (crc << 1) ^ 0x04c11db7;
        }
        else
        {
          crc = crc << 1;
        }
      }

      crctab[i] = crc;
    }
  }


  /**
   * Determine the CRC value for a block of bytes.  The number of bytes shall not
   * be less than 4.
   * @throw   IllegalArgumentException    In case the number of bytes is less than 4
   */
  static /*unsigned*/ int calculate ( /*unsigned char*/byte[] data,
      /*unsigned*/
      int len)
  {
    /*unsigned*/
    int result;

    if (len < 4)
    {
      throw new IllegalArgumentException (
          "More than 4 bytes required to calculate CRC; got " + len);
    }

    int i = 0;

    int j = data[i++] << 24;
    int k = 0xFF << 24;
    int l = j & k;
    result = l;

    j = data[i++] << 16;
    k = 0xFF << 16;
    l = j & k;
    result |= l;

    j = data[i++] << 8;
    k = 0xFF << 8;
    l = j & k;
    result |= l;

    j = data[i++];
    k = 0xFF;
    l = j & k;
    result |= l;

//d        System.out.println ("a: " + Integer. toHexString (result));
    result = ~result;
//d        System.out.println ("b: " + Integer. toHexString (result));
    len -= 4;

    while (len-- > 0)
    {
      int iData = data[i++];
      iData &= 0xFF;

      result = (result << 8 | iData) ^ crctab[result >>> 24];
//d            System.out.println ("c: " + Integer. toHexString (result));
    }

    return~result;
  }

  /*d
   public static void  main (String [] astr)
   throws Exception
   {
   byte[]  buf = new byte [35];
   {
    buf [0] = 'W';
    buf [1] = 'a';
    buf [2] = 's';
    buf [3] = ' ';
    buf [4] = 's';
    buf [5] = 'i';
    buf [6] = 'c';
    buf [7] = 'h';
    buf [8] = ' ';
    buf [9] = 'n';
    buf [10] = 'a';
    buf [11] = 'c';
    buf [12] = 'h';
    buf [13] = ' ';
    buf [14] = 'd';
    buf [15] = 'e';
    buf [16] = 'r';
    buf [17] = ' ';
    buf [18] = 'W';
    buf [19] = 'a';
    buf [20] = 'h';
    buf [21] = 'l';
    buf [22] = ' ';
    buf [23] = 'a';
    buf [24] = 'e';
    buf [25] = 'n';
    buf [26] = 'd';
    buf [27] = 'e';
    buf [28] = 'r';
    buf [29] = 'n';
    buf [30] = ' ';
    buf [31] = 'm';
    buf [32] = 'u';
    buf [33] = 's';
    buf [34] = 's';
    int  len = 35;
    System.out. println (buf + " -> " + Integer. toHexString (calculate (/*(unsigned char *)* /buf, len)));
      }
      }
    */

}
