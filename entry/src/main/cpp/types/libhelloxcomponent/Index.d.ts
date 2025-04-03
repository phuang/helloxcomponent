import { NodeContent } from '@ohos.arkui.node';

export const createNativeNode: (content: NodeContent, mode: number) => void;
export const setCurrentMode: (mode: number) => void;
export const onPageShow: () => void;
export const onPageHide: () => void;
