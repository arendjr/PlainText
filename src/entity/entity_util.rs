use super::EntityRef;

pub fn ref_difference(list1: &[EntityRef], list2: &[EntityRef]) -> Vec<EntityRef> {
    let mut result = vec![];
    for entity_ref in list1 {
        if !list2.contains(entity_ref) {
            result.push(*entity_ref)
        }
    }
    result
}

pub fn ref_union(list1: &[EntityRef], list2: &[EntityRef]) -> Vec<EntityRef> {
    let mut result: Vec<_> = list1.iter().copied().collect();
    for entity_ref in list2 {
        if !list1.contains(entity_ref) {
            result.push(*entity_ref)
        }
    }
    result
}
