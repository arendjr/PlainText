use super::GameObjectRef;

pub fn ref_difference(list1: &[GameObjectRef], list2: &[GameObjectRef]) -> Vec<GameObjectRef> {
    let mut result = vec![];
    for object_ref in list1 {
        if !list2.contains(object_ref) {
            result.push(*object_ref)
        }
    }
    result
}

pub fn ref_union(list1: &[GameObjectRef], list2: &[GameObjectRef]) -> Vec<GameObjectRef> {
    let mut result: Vec<_> = list1.iter().copied().collect();
    for object_ref in list2 {
        if !list1.contains(object_ref) {
            result.push(*object_ref)
        }
    }
    result
}
