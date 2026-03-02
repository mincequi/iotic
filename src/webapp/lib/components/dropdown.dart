import 'package:flutter/material.dart';

class DropdownItem {
  final String id;
  final String? name;

  DropdownItem({required this.id, this.name});
}

class Dropdown extends StatelessWidget {
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
  Widget build(BuildContext context) {
    return Row(
      //mainAxisAlignment: MainAxisAlignment.spaceBetween,
      crossAxisAlignment: CrossAxisAlignment.center,
      children: [
        const SizedBox(width: 6),
        Text(label,
            style: Theme.of(context).textTheme.labelSmall?.copyWith(
                fontWeight: FontWeight.bold,
                fontFeatures: const <FontFeature>[FontFeature.enable('smcp')])),
        const SizedBox(width: 6),
        Expanded(
            child: Card(
                margin: const EdgeInsets.all(0),
                child: InkWell(
                    onTap: () => _openMenu(context),
                    child: Row(children: [
                      const SizedBox(
                        width: 6,
                      ),
                      Expanded(
                        //width: 120,
                        child: Text(
                          selected?.name ?? selected?.id ?? "Select",
                          overflow: TextOverflow.ellipsis,
                          style: Theme.of(context).textTheme.bodyMedium,
                        ),
                      ),
                      const SizedBox(width: 6),
                      const Icon(
                        Icons.swap_vert,
                        size: 14,
                      ),
                      const SizedBox(width: 6),
                    ])))),
      ],
    );
  }

  void _openMenu(BuildContext context) async {
    final selected = await showMenu<String>(
      context: context,
      position: const RelativeRect.fromLTRB(100, 100, 0, 0),
      items: items.map((thing) {
        return PopupMenuItem<String>(
          value: thing.id,
          child: Text(thing.name ?? thing.id),
        );
      }).toList(),
    );

    if (selected != null) {
      onChanged(selected);
    }
  }
}
