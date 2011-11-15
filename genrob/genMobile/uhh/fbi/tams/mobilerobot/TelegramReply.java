package uhh.fbi.tams.mobilerobot;

import java.io.IOException;
import java.io.InputStream;
import java.text.DecimalFormat;

import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;

/**
 * Antwort-Telegramm der Robotersteuerung <tt>mobiled</tt> auf eine Anfrage.
 *
 * @see TelegramRequest
 * @since uhh.fbi.tams.mobilerobot 0.1
 * @author Hagen Stanek, Daniel Westhoff
 * @version 0.3
 */
class TelegramReply
{
  /** Logging. */
  private final ModuleLog out;

  /** Benutzt bei der Ausgabe des Telegrams mit {@link #dump() dump}. */
  private DecimalFormat dumpFormat = new DecimalFormat ("000");

  // Default-Konstruktor.
  private TelegramReply () {this.out = null;}

  /** Konstruktor. */
  TelegramReply (ModuleLog out)
  {
    this.out = out;
  }

  /**
   * Liest ein Telegramm von einem InputStream.
   *
   * @see BinarySocket
   */
  void readFrom (InputStream is)
      throws SocketCommunicationException
  {
    // Read first 8 byte (magic and size)
    byte[] abHeadBuffer = new byte[8];
    int iWriteAt = 0;
    {
      while (iWriteAt < 2 * 4)
      {
        int iBytesRead;
        try
        {
          iBytesRead = is.read (abHeadBuffer
                                , iWriteAt, 2 * 4 - iWriteAt); // magic and size
        }
        catch (IOException ioe)
        {
          throw new SocketCommunicationException (ioe);
        }
        if (iBytesRead == -1)
        {
          throw new SocketCommunicationException
              ("Unexpected end of stream while reading telegram header");
        }
        iWriteAt += iBytesRead;
      }
    }

    // Check magic
    {
      if (Telegram.getInteger (abHeadBuffer, 0) != Telegram.MAGIC)
      {
        throw new SocketCommunicationException ("Invalid magic");
      }
    }

    // Check size
    int iByteSize;
    {
      iByteSize = Telegram.getInteger (abHeadBuffer, 4);
      if (iByteSize > Telegram.BYTESIZE_MAX)
      {
        throw new SocketCommunicationException ("Invalid telegram size (" + iByteSize
                                   + ";  only up to "
                                   + Telegram.BYTESIZE_MAX + " allowed)");
      }
      if (iByteSize < 16)
      {
        throw new SocketCommunicationException ("Invalid telegram size (" + iByteSize
                                   + ";  minimum of 16 expected)");
      }
      if (iByteSize % 4 != 0)
      {
        throw new SocketCommunicationException ("Invalid telegram size (" + iByteSize
                                   + ";  multiple of 4 expected)");
      }
    }

    // Allocate receive buffer
    m_abBuffer = new byte[iByteSize];

    // Copy bytes already read into the new buffer
    {
      for (int i = 0; i < 8; ++i)
      {
        m_abBuffer[i] = abHeadBuffer[i];
      }
    }

    // Read remaining bytes
    {
      while (iWriteAt < iByteSize)
      {
        int iBytesRead;
        try
        {
          iBytesRead = is.read (m_abBuffer
                                , iWriteAt, iByteSize - iWriteAt); // magic and size
        }
        catch (IOException ioe)
        {
          throw new SocketCommunicationException (ioe);
        }
        if (iBytesRead == -1)
        {
          throw new SocketCommunicationException
              ("Unexpected end of stream while reading telegram tail");
        }
        iWriteAt += iBytesRead;
      }
    }

    // Check CRC
    {
      // Calculate CRC and get the received one
      int iCRCPos = iByteSize - 4;
      int iCRC_Calculated = CRC.calculate (m_abBuffer
                                           , iCRCPos); // exclude the last which is the CRC
      int iCRC_Received = Telegram.getInteger (m_abBuffer, iCRCPos);

      if (iCRC_Received != iCRC_Calculated)
      {
        throw new SocketCommunicationException
            ("Calculated telegram CRC does not match the received one");
      }
    }

    // Calculate number of available slots
    {
      int iCount = m_abBuffer.length / 4;
      iCount -= 4; // magic, size, command, CRC
      m_iDataSlots = iCount;
    }
  }


  private byte[] m_abBuffer;

  private int m_iDataSlots = 0;

  int getCommand ()
  {
    return Telegram.getInteger (m_abBuffer, 8);
  }


  int getNumberOfSlots ()
  {
    return m_iDataSlots;
  }


  int getInteger (int iSlotOffset)
  {
    if (iSlotOffset >= m_iDataSlots)
    {
      throw new IndexOutOfBoundsException ("Invalid slot offset " + iSlotOffset
                                           + ";  number of slots is "
                                           + m_iDataSlots);
    }

    return Telegram.getInteger (m_abBuffer, iSlotOffset * 4 + 12);
  }


  float getFloat (int iSlotOffset)
  {
    if (iSlotOffset >= m_iDataSlots)
    {
      throw new IndexOutOfBoundsException ("Invalid slot offset " + iSlotOffset
                                           + ";  number of slots is "
                                           + m_iDataSlots);
    }

    return Telegram.getFloat (m_abBuffer, iSlotOffset * 4 + 12);
  }


  void dump ()
  {
    if (out.module)
    {
      // Dump command
      out.module (this,
                  "Antwort empfangen: "
                  + Convert.toHex (this.getCommand ()));

      // Dump data
      int count = getNumberOfSlots ();

      for (int i = 0; i < count; ++i)
      {
        out.module (this,
                    "Data ["
                    + dumpFormat.format (i)
                    + "]: "
                    + Convert.toDecAndHex (this.getInteger (i)));
      }
    }
  }

}
