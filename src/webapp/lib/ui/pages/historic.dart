import 'package:flutter/material.dart';

class Historic extends StatelessWidget {
  const Historic({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    /*
    return const Card(
        child: ListTile(
      title: Text(
        "Historic",
        textScaleFactor: 2.0,
      ),
    ));
    */

    return const Card(
      child: Column(children: [
        ListTile(
          leading: Icon(Icons.ac_unit_sharp, size: 32),
          title: Text(
            "Historic",
            textScaleFactor: 2.0,
          ),
        ),
      ]),
    );
  }
}
