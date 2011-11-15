package uhh.fbi.tams.mobilerobot;

import java.io.OutputStream;

import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;

/**
 * Kommando-Telegramm, das an die Robotersteuerung <i>mobiled</i>
 * gesendet wird.
 */
class TelegramRequest
{
  /** Daten. */
  private int numberOfDataSlots;

  /** Gr&ouml;sse in Bytes: Slots plus Magic, Size, Command and CRC. */
  private int sizeInByte;

  /** Puffer. */
  private byte[] buffer;

  private final int command;

  /** Konstruktor. */
  TelegramRequest (int command, int numberOfSataSlots)
  {
    this.command = command;
    this.numberOfDataSlots = numberOfSataSlots;

    // slots plus magic, size, command and CRC
    this.sizeInByte = numberOfSataSlots * 4 + 16;

    if (sizeInByte > Telegram.BYTESIZE_MAX)
    {
      throw new IllegalArgumentException
          ("Anzahl der Daten gr\u00F6\u00DFer als maximale Telegramgr\u00f6\u00DFe.");
    }

    buffer = new byte[sizeInByte];

    Telegram.putInteger (buffer, 0, Telegram.MAGIC);
    Telegram.putInteger (buffer, 4, sizeInByte);
    Telegram.putInteger (buffer, 8, command);
  }


  /** Neues Integer-Datum einf&uuml;gen. */
  void putInteger (int iSlotOffset, int iValue)
  {
    if (iSlotOffset >= numberOfDataSlots)
    {
      throw new IndexOutOfBoundsException ("Invalid slot offset " + iSlotOffset
                                           + ";  number of slots is "
                                           + numberOfDataSlots);
    }

    Telegram.putInteger (buffer, iSlotOffset * 4 + 12, iValue);
  }

  /** Neues Float-Datum einf&uuml;gen. */
  void putFloat (int iSlotOffset, float fValue)
  {
    if (iSlotOffset >= numberOfDataSlots)
    {
      throw new IndexOutOfBoundsException ("Invalid slot offset " + iSlotOffset
                                           + ";  number of slots is "
                                           + numberOfDataSlots);
    }

    Telegram.putFloat (buffer, iSlotOffset * 4 + 12, fValue);
  }

  /** Telegram auf Ausgabe-Strom schreiben. */
  void writeTo (OutputStream os)
      throws SocketCommunicationException
  {
    calculateCRC ();
    try
    {
      os.write (buffer);
    }
    catch (java.io.IOException ioe)
    {
      throw new SocketCommunicationException (ioe);
    }
  }

  /** Checksumme berechnen. */
  private void calculateCRC ()
  {
    int iSizeWithoutCRC = sizeInByte - 4; // exclude the last which is the CRC
    int iCRC = CRC.calculate (buffer, iSizeWithoutCRC);

    Telegram.putInteger (buffer, iSizeWithoutCRC, iCRC);
  }

  /** Kommando-ID zur&uuml;ckgeben. */
  int getCommand ()
  {
    return this.command;
  }
}
