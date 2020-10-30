# mz-transformerstation

## Payload


### Format

Byte|description
-|-
0|node id
1|water conductivity `[0..255]`
2|distance (high byte)
3|distance (low byte)
4|pressure (high byte)
5|pressure (low byte)

### Decoding

```js
function Decoder(bytes, port) {
  // Decode an uplink message from a buffer
  // (array) of bytes to an object of fields.
  var decoded = {};

   if (port === 1){
    
    decoded.node_id = bytes[0];
    decoded.conductivity = bytes[1];
    decoded.distance = (bytes[2]<<8)+bytes[3];
    decoded.pressure = (bytes[4]<<8)+bytes[5];
   }

  return decoded;
}
```