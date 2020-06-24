use super::GameObjectRef;

pub fn ref_difference(
    list1: &Vec<GameObjectRef>,
    list2: &Vec<GameObjectRef>,
) -> Vec<GameObjectRef> {
    let mut result = vec![];
    for object_ref in list1 {
        if !list2.contains(object_ref) {
            result.push(*object_ref)
        }
    }
    result
}

pub fn ref_union(list1: &Vec<GameObjectRef>, list2: &Vec<GameObjectRef>) -> Vec<GameObjectRef> {
    let mut result = list1.clone();
    for object_ref in list2 {
        if !list1.contains(object_ref) {
            result.push(*object_ref)
        }
    }
    result
}
