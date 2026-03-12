import 'package:flutter/material.dart';
import 'dart:ui';

class DropdownItem {
  final String id;
  final String? name;

  DropdownItem({required this.id, this.name});
}

class Dropdown extends StatefulWidget {
  const Dropdown({
    super.key,
    required this.label,
    required this.items,
    required this.selected,
    required this.onChanged,
  });

  final String label;
  final List<DropdownItem> items;
  final DropdownItem? selected;
  final ValueChanged<String> onChanged;

  @override
  State<Dropdown> createState() => _DropdownState();
}

class _DropdownState extends State<Dropdown> {
  final LayerLink _layerLink = LayerLink();
  OverlayEntry? _overlayEntry;

  void _openMenu() {
    _overlayEntry = _createOverlayEntry();
    Overlay.of(context).insert(_overlayEntry!);
  }

  void _closeMenu() {
    _overlayEntry?.remove();
    _overlayEntry = null;
  }

  OverlayEntry _createOverlayEntry() {
    RenderBox renderBox = context.findRenderObject() as RenderBox;
    var size = renderBox.size;
    var offset = renderBox.localToGlobal(Offset.zero);

    // Compute max height: from top of screen to bottom, minus some padding
    double maxHeight = MediaQuery.of(context).size.height - offset.dy - 20;

    return OverlayEntry(
      builder: (context) => Positioned(
        left: offset.dx,
        width: size.width, // same width as card
        top: offset.dy + size.height, // directly below the card
        child: Material(
          color: Colors.transparent,
          child: ConstrainedBox(
            constraints: BoxConstraints(
              maxHeight: maxHeight, // allow scrolling if too many items
            ),
            child: Card(
              //color: Colors.black.withAlpha(127),
              elevation: 4,
              child: ListView(
                padding: EdgeInsets.zero,
                shrinkWrap: true,
                children: widget.items.map((item) {
                  return InkWell(
                    onTap: () {
                      widget.onChanged(item.id);
                      _closeMenu();
                    },
                    child: Padding(
                      padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
                      child: Text(
                        item.name ?? item.id,
                        style: const TextStyle(color: Colors.white),
                      ),
                    ),
                  );
                }).toList(),
              ),
            ),
          ),
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return CompositedTransformTarget(
      link: _layerLink,
      child: Row(
        crossAxisAlignment: CrossAxisAlignment.center,
        children: [
          const SizedBox(width: 6),
          Text(widget.label,
              style: Theme.of(context).textTheme.labelSmall?.copyWith(
                  //backgroundColor: Colors.black.withAlpha(127),
                  fontWeight: FontWeight.bold,
                  fontFeatures: const <FontFeature>[FontFeature.enable('smcp')])),
          const SizedBox(width: 6),
          Expanded(
            child: Card(
              //color: Colors.black.withAlpha(127),
              margin: EdgeInsets.zero,
              child: InkWell(
                onTap: () {
                  if (_overlayEntry == null) {
                    _openMenu();
                  } else {
                    _closeMenu();
                  }
                },
                child: Row(
                  children: [
                    const SizedBox(width: 6),
                    Expanded(
                      child: Text(
                        widget.selected?.name ?? widget.selected?.id ?? "Select",
                        overflow: TextOverflow.ellipsis,
                        style: Theme.of(context).textTheme.bodyMedium?.copyWith(color: Colors.white),
                      ),
                    ),
                    const SizedBox(width: 6),
                    const Icon(
                      Icons.swap_vert,
                      size: 16,
                      color: Colors.white,
                    ),
                    const SizedBox(width: 6),
                  ],
                ),
              ),
            ),
          ),
        ],
      ),
    );
  }
}
