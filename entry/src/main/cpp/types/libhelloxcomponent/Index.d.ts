import { NodeContent } from '@ohos.arkui.node';

export const createNativeNode: (content: NodeContent, delegated: boolean) => void;
export const setDelegatedCompositing: (enable: boolean) => void;
export const onPageShow: () => void;
export const onPageHide: () => void;
