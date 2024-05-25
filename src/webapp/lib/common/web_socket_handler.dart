abstract class WebSocketHandler {
  // Return `true` if the message is handled
  bool onMessage(String id, Map<String, dynamic> entry);
}
