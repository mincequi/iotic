import 'dart:math';
import 'package:flutter/material.dart';

class EnergyFlow extends StatelessWidget {
  const EnergyFlow({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return CustomPaint(
      painter: PathPainter(context),
    );
  }
}

class PathPainter extends CustomPainter {
  PathPainter(this._context);
  final BuildContext _context;

  var arrowWidth = 8.0;

  double degToRad(double deg) => deg * (pi / 180.0);

  @override
  void paint(Canvas canvas, Size size) {
    Paint strokePaint = Paint()
      ..color = Theme.of(_context).colorScheme.onBackground
      ..style = PaintingStyle.stroke
      ..strokeWidth = arrowWidth;

    Paint fillPaint = Paint()
      ..color = Theme.of(_context).colorScheme.onBackground
      ..style = PaintingStyle.fill;

    Path plantToGrid = Path();
    plantToGrid.arcTo(
        Rect.fromCenter(
          center: Offset(arrowWidth, 0.0),
          width: 112 - 2 * arrowWidth,
          height: 72,
        ),
        0.0,
        degToRad(90.0),
        true);
    canvas.drawPath(plantToGrid, strokePaint);

    Path gridArrow = Path();
    gridArrow.moveTo(0, size.height / 2);
    gridArrow.lineTo(arrowWidth, size.height / 2 + arrowWidth);
    gridArrow.lineTo(arrowWidth, size.height / 2 - arrowWidth);
    gridArrow.lineTo(0, size.height / 2);
    canvas.drawPath(gridArrow, fillPaint);

    /*
    Path gridToHome = Path();
    gridToHome.arcTo(
        Rect.fromCenter(
          center: Offset(0.0, 72.0 - arrowWidth),
          width: 112,
          height: 72 - 2 * arrowWidth,
        ),
        0.0,
        degToRad(-90.0),
        true);
    canvas.drawPath(gridToHome, strokePaint);
    */
    Path homeArrow = Path();
    homeArrow.moveTo(size.width / 2, size.height);
    homeArrow.lineTo(size.width / 2 + arrowWidth, size.height - arrowWidth);
    homeArrow.lineTo(size.width / 2 - arrowWidth, size.height - arrowWidth);
    homeArrow.lineTo(size.width / 2, size.height);
    canvas.drawPath(homeArrow, fillPaint);

    canvas.drawRect(
        Rect.fromLTWH(
          size.width / 2 - arrowWidth / 2,
          0,
          arrowWidth,
          size.height - arrowWidth,
        ),
        fillPaint);

    Paint contourPaint = Paint()
      ..color = Theme.of(_context).canvasColor
      ..style = PaintingStyle.stroke
      ..strokeWidth = 2.0;
    canvas.drawLine(
      Offset(size.width / 2 - arrowWidth / 2 - 1, 0),
      Offset(size.width / 2 - arrowWidth / 2 - 1, size.height - arrowWidth),
      contourPaint,
    );
    canvas.drawLine(
      Offset(size.width / 2 + arrowWidth / 2 + 1, 0),
      Offset(size.width / 2 + arrowWidth / 2 + 1, size.height - arrowWidth),
      contourPaint,
    );
  }

  @override
  bool shouldRepaint(CustomPainter oldDelegate) => true;
}
